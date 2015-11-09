/**
 * 화일명 : ax88976b.c
 * 설  명 : 이더넷칩 AX88796B를 제어한다.
 * 작성자 : 장형기 에프에이리눅스(주)  tsheaven@falinux.com
 * 작성일 : 2007년 03월 23일
 * 저작권 : 에프에이리눅스(주)
 * 주  의 : 이 소스는 ax88796b의 8051소스를 EZ-S2440 보드에 맞게 수정한 것이다.
 * 
 * 수  정 : 2009.04.30  리눅스와의 tftp 통신시 문제를 강민호님께서 해결 
 */
#include <config.h>

#include <common.h>
#include <command.h>
#include <malloc.h>
#include <net.h>
#include <miiphy.h>
#include "ax88796b.h"

#define AX88796B_DBG 0

static u32 AX_ADDR_SHIFT = 1;	

/**
 * 설명 : Byte 단위 read.
 */
static inline unsigned char ax_readb(unsigned long addr)
{
	unsigned char v;

	v = *((volatile unsigned char*)(AX88796B_BASE + addr));
	return v;
}

/**
 * 설명 : Byte 단위 write.
 */
static inline void ax_writeb(unsigned char value, unsigned long addr)
{
    *((volatile unsigned char*)(AX88796B_BASE + addr)) = value;
}
/**
 * 설명 : Word 단위 read.
 */
static inline unsigned short ax_readw(unsigned long addr)
{
	unsigned short v;

	v = *((volatile unsigned short*)(AX88796B_BASE + addr));
	return v;
}
/**
 * 설명 : Word 단위 write.
 */
static inline void ax_writew(unsigned short value, unsigned long addr)
{
    *((volatile unsigned short*)(AX88796B_BASE + addr)) = value;
}

#if AX88796B_DBG
static void pageprint(struct ethdevice *dev)
{
	unsigned char oriCR, tmp = 0;
	int i, j;	

	oriCR = ax_readb(E8390_CMD);

	printf("\n");

	for (i = 0; i < 4; i++) {
		tmp = ax_readb(E8390_CMD) & 0x3F;
		tmp |= (i << 6);
		ax_writeb(tmp, E8390_CMD);
		
		printf("\npage %02X:\n", i);

		for (j = 0; j < 0x1F; j++) {
			printf("%02X:%02X ", j, ax_readb(EI_SHIFT(j)));

			if((j % 8) == 7)
				printf("\n"); 
		}
		
		printf("\n"); 	
	}
				
	ax_writeb(oriCR, E8390_CMD);
	
}
#endif


/*======================================================================
    MII interface support
======================================================================*/
#define MDIO_SHIFT_CLK		0x01
#define MDIO_DATA_WRITE0	0x00
#define MDIO_DATA_WRITE1	0x08
#define MDIO_DATA_READ		0x04
#define MDIO_MASK			0x0f
#define MDIO_ENB_IN			0x02

static void mdio_sync(void)
{
    int bits;

    for (bits = 0; bits < 32; bits++) {
		ax_writeb(MDIO_DATA_WRITE1, AX88796_MII_EEPROM);
		ax_writeb(MDIO_DATA_WRITE1 | MDIO_SHIFT_CLK, AX88796_MII_EEPROM);
    }
}

static void mdio_clear(void)
{
    int bits;

    for (bits = 0; bits < 16; bits++) {
		ax_writeb(MDIO_DATA_WRITE0, AX88796_MII_EEPROM);
		ax_writeb(MDIO_DATA_WRITE0 | MDIO_SHIFT_CLK, AX88796_MII_EEPROM);
    }
}

static int mdio_read(int phy_id, int loc)
{
    unsigned int cmd = (0xf6<<10)|(phy_id<<5)|loc;
    int i, retval = 0;

	mdio_clear();
    mdio_sync();

    for (i = 14; i >= 0; i--) {
		int dat = (cmd&(1<<i)) ? MDIO_DATA_WRITE1 : MDIO_DATA_WRITE0;
		ax_writeb(dat, AX88796_MII_EEPROM);
		ax_writeb(dat | MDIO_SHIFT_CLK, AX88796_MII_EEPROM);
    }
    for (i = 19; i > 0; i--) {
		ax_writeb(MDIO_ENB_IN, AX88796_MII_EEPROM);
		retval = (retval << 1) | ((ax_readb(AX88796_MII_EEPROM) & MDIO_DATA_READ) != 0);
		ax_writeb(MDIO_ENB_IN | MDIO_SHIFT_CLK, AX88796_MII_EEPROM);
    }
    return (retval>>1) & 0xffff;
}

static void mdio_write(int phy_id, int loc, int value)
{
    unsigned int cmd = (0x05<<28)|(phy_id<<23)|(loc<<18)|(1<<17)|value;
    int i;

	mdio_clear();
    mdio_sync();

    for (i = 31; i >= 0; i--) {
	int dat = (cmd&(1<<i)) ? MDIO_DATA_WRITE1 : MDIO_DATA_WRITE0;
		ax_writeb(dat, AX88796_MII_EEPROM);
		ax_writeb(dat | MDIO_SHIFT_CLK, AX88796_MII_EEPROM);
    }
    for (i = 1; i >= 0; i--) {
		ax_writeb(MDIO_ENB_IN, AX88796_MII_EEPROM);
		ax_writeb(MDIO_ENB_IN | MDIO_SHIFT_CLK, AX88796_MII_EEPROM);
    }
}

static unsigned char XmitPage;

/**
 * 설명 : AX88796 RESET 함수이다.
 */
void ax88796b_reset(void)
{
	unsigned char value;

	value = ax_readb(EN0_RESET);
	ax_writeb(value, EN0_RESET);

	value = ax_readb(EN0_ISR);
	if(value != ENISR_RESET)
		printf("AX88796B Reset Failure\r\n");
}

/**
 * 설명 : packet을 전송.
 *        전송 가능한 상태인지 확인한 후 EN0_DATAPORT에 packet을 반복적으로 기록.
 * 매개 : pTxBuf : 전송할 패켓 어드레스 
 *        len      : 패켓 크기  
 * 반환 : 
 * 주의 : 없음 
 */
static int ax88796b_send (struct eth_device *dev, volatile void *packet, int length)
{
	unsigned char	*pBuf = (unsigned char *)packet;
	unsigned char	TxStartPage;
	unsigned short	i, TxLen=0;
	unsigned short	data;

	ax_writeb((E8390_START | E8390_PAGE0 | E8390_NODMA), E8390_CMD);
	ax_writeb(ENISR_RDC,	EN0_ISR);

	if (XmitPage == 0)
	{
		TxStartPage = NESM_START_PG;
		XmitPage = 1;
	}
	else
	{
		TxStartPage = NESM_START_PG + 6;
		XmitPage = 0;
	}

	if(length < IEEE_8023_MIN_FRAME)
		TxLen = IEEE_8023_MIN_FRAME;
	else {
		TxLen = (unsigned short)length;
	}

	ax_writeb((TxLen & 0xff),		EN0_RCNTLO);
	ax_writeb((TxLen & 0xff00)>>8,	EN0_RCNTHI);
	ax_writeb(0,					EN0_RSARLO);
	ax_writeb(TxStartPage,			EN0_RSARHI);

	ax_writeb((E8390_START | E8390_RWRITE), E8390_CMD);

	for (i=0; i<TxLen; i+=2)
	{
		data = (unsigned short)*(pBuf+i) + ( (unsigned short)*(pBuf+i+1) << 8 );
		ax_writew(data, EN0_DATAPORT);
	}

	while((ax_readb(EN0_ISR) & ENISR_RDC)==0);
		ax_writeb(ENISR_RDC, EN0_ISR);

	ax_writeb((E8390_START | E8390_PAGE0 | E8390_NODMA), E8390_CMD);

	ax_writeb((TxLen & 0xff),		EN0_TCNTLO);
	ax_writeb((TxLen & 0xff00)>>8,	EN0_TCNTHI);
	ax_writeb(TxStartPage,			EN0_TPSR);

	ax_writeb((E8390_START | E8390_PAGE0 | E8390_TRANS), E8390_CMD);

	ax_writeb(ENINT_MASK, EN0_IMR);

	return TRUE;
}
/*
 * ----------------------------------------------------------------------------
 * Function Name: ax_get_hdr
 * Purpose: Grab the 796b specific header
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
static void ax_get_hdr(struct ax_pkt_hdr *hdr, int ring_page)
{
	u16 tmp;

	// read 4 bytes header
	ax_writeb((E8390_START | E8390_PAGE0 | E8390_NODMA), E8390_CMD);
	ax_writeb(sizeof(struct ax_pkt_hdr),	EN0_RCNTLO);
	ax_writeb(0,			EN0_RCNTHI);
	ax_writeb(0, 			EN0_RSARLO);
	ax_writeb(ring_page,	EN0_RSARHI);
	// set remote read
	ax_writeb((E8390_START | E8390_PAGE0 | E8390_RREAD), E8390_CMD);

	while (( ax_readb(EN0_SR) & 0x20) ==0);
	
	for(tmp=0; tmp < (sizeof(struct ax_pkt_hdr)>>1); tmp++)
	{
		*((u16 *)hdr + tmp)= ax_readw(EN0_DATAPORT);
	}
	ax_writeb(ENISR_RDC, EN0_ISR);	/* Ack intr. */
}
/**
 * 설명 : 한 프레임 패킷을 수신한다. 
 * 매개 : 
 * 반환 : 값을 읽을 경우는 길이 / 없으면 0 
 * 주의 : 
 */
static int ax88796b_recv (struct eth_device *dev)
{
	struct ax_pkt_hdr	rxframe;
	unsigned short	curr_offset;
	unsigned char *addr;
	unsigned char	rxing_page, this_frame, next_frame;
	unsigned char	isr;	//, value;
	int				i,tmp, total=0;

	isr = ax_readb(EN0_ISR);
	if ((isr & (ENISR_RX | ENISR_OVER)) == 0) 
		return 0;
	
	while(1)
	{
		int packetlength;
		ax_writeb((E8390_START | E8390_PAGE1 | E8390_NODMA), E8390_CMD);
		rxing_page = ax_readb(EN1_CURPAG);
		ax_writeb((E8390_START | E8390_PAGE0 | E8390_NODMA), E8390_CMD);

		this_frame = ax_readb(EN0_BOUNDARY)+1;
		if(this_frame >= NESM_STOP_PG)
			this_frame = NESM_RX_START_PG;

		//Todo: Page compare
		if(this_frame == rxing_page) break;

		curr_offset  = (this_frame << 8) + sizeof(struct ax_pkt_hdr);
		ax_get_hdr(&rxframe, this_frame);

		packetlength = rxframe.count - sizeof(struct ax_pkt_hdr);
		next_frame = this_frame + 1 + ((packetlength+4)>>8);

		if( (rxframe.next != next_frame)		&&
			(rxframe.next != next_frame + 1)	&&
			(rxframe.next != next_frame -     (NESM_STOP_PG-NESM_RX_START_PG)) 	&&
			(rxframe.next != next_frame + 1 - (NESM_STOP_PG-NESM_RX_START_PG)) )
		{
//			printf("this packet droped!\r\n");
			ax_writeb(rxing_page-1, EN0_BOUNDARY);
			continue;
		}

		if((packetlength < 60) || (packetlength > 1518 ))
		{
			printf("this packet too short or long : %d\r\n", packetlength);
		}
		else if(rxframe.status & ENRSR_RXOK)
		{

			ax_writeb((E8390_START | E8390_PAGE0 | E8390_NODMA), E8390_CMD);

			ax_writeb(packetlength  & 0xff,		EN0_RCNTLO);
			ax_writeb((packetlength & 0xff00)>>8,	EN0_RCNTHI);
			ax_writeb(curr_offset   & 0xff,		EN0_RSARLO);
			ax_writeb((curr_offset  & 0xff00)>>8,	EN0_RSARHI);

			// set remote read
			ax_writeb((E8390_START | E8390_PAGE0 | E8390_RREAD), E8390_CMD);

			//wait for ready state for DMA read
			while((ax_readb(EN0_SR) & ENDSR_RD_RDY)==0);

			addr = (unsigned char *) NetRxPackets[0];
			for (i=0; i<(packetlength>>1); i++)
			{
	 			*((u16 *)addr + i) = ax_readw(EN0_DATAPORT);
			}				
			if (packetlength & 0x01)
			{	
				addr[packetlength-1] = ax_readb(EN0_DATAPORT);
			}		
#if AX88796B_DBG
			printf("Step #0f... \n");

			printf("rxpkt dump\n");
			addr = (unsigned char *) NetRxPackets[0];
			tmp = packetlength >> 1;
			for (i = 0; i < tmp; i++) {
				printf("%04X ", *((u16 *)addr + i));

				if ((i % 8) == 7)
					printf("\n");
			}
			printf("\n");

			printf("(rxlen = 0x%04x)\n", packetlength);
#endif
			ax_writeb(ENISR_RDC, EN0_ISR);
		}
		else
		{
			//printf("this packet droped : 0x%x\r\n", rxframe.status);
		}

		if (packetlength != 0) {
			/* Pass the packet up to the protocol layers. */
			NetReceive (NetRxPackets[0], packetlength);
		}

		next_frame = rxframe.next;
		ax_writeb(next_frame-1, EN0_BOUNDARY);

		total += packetlength;
		
		break;  //
	}

	return 0;
}

/// 2010.09.01 이더넷 허버의 문제로 100FULL이 접속안될 경우가 있어서 수정함
/// 100FULL로 사용할 경우 테스트 후 사용할 것  
//static int media_mode = MEDIA_10HALF;
// 2011.03.31 이더넷 문제가있어 100M 로 처리함
static int media_mode = MEDIA_100HALF;

/*
 * ----------------------------------------------------------------------------
 * Function Name: ax88796_PHY_init
 * Purpose:
 * Params:
 * Returns:
 * Note:
 * ----------------------------------------------------------------------------
 */
void ax88796_PHY_init(void)
{
	u8 tmp_data;

	/* Enable AX88796B FOLW CONTROL */
	ax_writeb(ENFLOW_ENABLE, EN0_FLOW);

	/* Enable PHY PAUSE */
	mdio_write(0x10,0x04,(mdio_read(0x10,0x04) | 0x400));
	mdio_write(0x10,0x00,0x1200);

	/* Enable AX88796B TQC */
	tmp_data = ax_readb(EN0_MCR);
	ax_writeb( tmp_data | ENTQC_ENABLE, EN0_MCR);

	/* Enable AX88796B Transmit Buffer Ring */
	ax_writeb(E8390_NODMA+E8390_PAGE3+E8390_STOP, E8390_CMD);
	ax_writeb(ENTBR_ENABLE, EN3_TBR);
	ax_writeb(E8390_NODMA+E8390_PAGE0+E8390_STOP, E8390_CMD);

	switch (media_mode) {
	default:
	case MEDIA_AUTO:
//		printf("AX88796B: The media mode is autosense.\n");
		break;

	case MEDIA_100FULL:
//		printf("AX88796B: The media mode is forced to 100full.\n");
		mdio_write(0x10,0x00,0x2300);
		break;

	case MEDIA_100HALF:
//		printf("AX88796B: The media mode is forced to 100half.\n");
		mdio_write(0x10,0x00,0x2200);
		break;

	case MEDIA_10FULL:
//		printf("AX88796B: The media mode is forced to 10full.\n");
		mdio_write(0x10,0x00,0x0300);
		break;

	case MEDIA_10HALF:
//		printf("AX88796B: The media mode is forced to 10half.\n");
		mdio_write(0x10,0x00,0x0200);
		break;
	}
}

/**
 * 역할 : AX88796B는 인터럽트를 쓰지 않는 IO모드로 초기화 한다. 
 * 매개 : 
 * 반환 : 성공하면 TRUE 실패하면 FALSE를 반환한다. 
 * 주의 : 
 */
static int ax88796b_init (struct eth_device *dev, bd_t * bd)
{
	unsigned char i;
	
	// reset ax88796 chip
	ax88796b_reset();
	
	// stop MAC, Page0
	ax_writeb((E8390_STOP | E8390_NODMA | E8390_PAGE0), E8390_CMD);

	mdelay(5);

	ax_writeb(0x48|ENDCFG_WTS, EN0_DCFG);	// 0x49

	// clear the remote byte count registers
	ax_writeb(0, EN0_RCNTLO);
	ax_writeb(0, EN0_RCNTHI);

	// set to monitor and loopback mode
//	ax_writeb(ENRCR_MONITOR,   EN0_RXCR);
	ax_writeb(ENRCR_BROADCAST, EN0_RXCR);
//	ax_writeb(ENTCR_LOCAL,  	EN0_TXCR);
	ax_writeb(ENTCR_FDU,       EN0_TXCR);

	// set the transmit page and receive ring
	ax_writeb(NESM_START_PG,	EN0_TPSR);
	ax_writeb(NESM_RX_START_PG,	EN0_STARTPG);
//	ax_writeb(NESM_RX_START_PG,	EN0_BOUNDARY );
	ax_writeb(NESM_STOP_PG-1,	EN0_BOUNDARY );
	ax_writeb(NESM_STOP_PG,		EN0_STOPPG);

	// clear the pending interrupts and mask
	ax_writeb(0xff,	EN0_ISR);
	ax_writeb(0,	EN0_IMR);

	// copy the station address into the DS8390 registers
	ax_writeb((E8390_STOP | E8390_NODMA | E8390_PAGE1), E8390_CMD);
	mdelay(5);

	//Write out the current receive buffer to receive into
	ax_writeb(NESM_RX_START_PG + 1, EN1_CURPAG);

	// set MAC address
	printf("\nAX88796B MAC  : [ ");
	for (i=0; i<ETHER_ADDR_LEN; i++)
	{   
		ax_writeb( dev->enetaddr[i] , (i+1)<<1);
		if(ax_readb((i+1)<<1) != dev->enetaddr[i])
		{
			ax_writeb(dev->enetaddr[i], (i+1)<<1);
			if(ax_readb((i+1)<<1) != dev->enetaddr[i])
			{
				printf("  Mac address set failurei ]\n");
				return FALSE;
			}
		}
		printf("%02X ", ax_readb((i+1)<<1));		
	}
	printf("]\n");

	// change to page0
	ax_writeb((E8390_STOP | E8390_NODMA | E8390_PAGE0), E8390_CMD);

	ax88796_PHY_init();
	ax_writeb(0xff,  EN0_ISR);
	ax_writeb(ENISR_ALL, EN0_IMR);
	ax_writeb(E8390_NODMA+E8390_PAGE0+E8390_START, E8390_CMD);
	ax_writeb(E8390_TXCONFIG, EN0_TXCR); /* xmit on. */
	/* 3c503 TechMan says rxconfig only after the NIC is started. */
	ax_writeb(E8390_RXCONFIG, EN0_RXCR); /* rx on,  */

	mdelay(2000);		
	printf("AX88796B_Init : OK!\r\n\n");

	return TRUE;
}


static void ax88796b_halt (struct eth_device *dev)
{
#if AX88796B_DBG
	printf("ax88796b_halt: Beginning...\n"); 
#endif
	ax_writeb(EN0_STOPPG, E8390_CMD);
#if AX88796B_DBG
	printf("ax88796b_halt: End...\n"); 
#endif

}


/*
===========================================================================
<<<<<<                 Exported SubProgram Bodies              >>>>>>
===========================================================================
*/
int ax88796b_initialize (bd_t *bis)
{
	struct eth_device *dev;
	struct ax88796b_private *ax_local;

#if AX88796B_DBG
	printf("ax88796b_initialize: Beginning...\n"); 
#endif
	dev = (struct eth_device *)malloc (sizeof *dev);
	if (NULL == dev)
		return 0;

	ax_local = (struct ax88796b_private *)malloc (sizeof *ax_local);
	if (NULL == ax_local)
		return 0;

	memset (dev, 0, sizeof *dev);
	memset (ax_local, 0, sizeof *ax_local);

	sprintf (dev->name, "ax88796b");
	dev->iobase = AX88796B_BASE;
	dev->priv = ax_local;
	dev->init = ax88796b_init;
	dev->halt = ax88796b_halt;
	dev->send = ax88796b_send;
	dev->recv = ax88796b_recv;

	ax88796b_reset ();

#if AX88796B_DBG
	pageprint(dev);
#endif

	eth_register (dev);

#if AX88796B_DBG
	printf("ax88796b_initialize: End...\n"); 
#endif
	return 1;

}
