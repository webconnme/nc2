/*
 * (C) Copyright 2010
 * jung hyun kim, Nexell Co, <jhkim@nexell.co.kr>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <common.h>
#include <spi.h>
#include <malloc.h>
#include <platform.h>

#if (0)
#define DBGOUT(msg...)		{ printf("spi_eeprom: " msg); }
#else
#define DBGOUT(msg...)
#endif

#define ERROUT(msg...)		{ printf("spi_eeprom: line=%d ", __LINE__); printf(msg); }

/*----------------------------------------------------------------------------*/
#define	EEPROM_ERASE_SIZE		64*1024

/*----------------------------------------------------------------------------*/
#define	EEPROM_PAGE_MASK		 (EEPROM_ERASE_SIZE - 1)
#define	EEPROM_ERASE_MASK		~(EEPROM_ERASE_SIZE - 1)

/*----------------------------------------------------------------------------*/
#define SPI0_GPIO_GROUP			0	// GPIO A
#define SPI0_FRM_GPIO_NUM		28
#define SPI0_CLK_GPIO_NUM		29
#define SPI0_RXD_GPIO_NUM		30
#define SPI0_TXD_GPIO_NUM		31

#define WP_GPIO_GROUP			CONFIG_SPI_EEPROM_WP_GROUP
#define WP_GPIO_BITNUN			CONFIG_SPI_EEPROM_WP_PADNUM

#define SER_WREN				0x06		// Set Write Enable Latch
#define SER_WRDI				0x04		// Reset Write Enable Latch
#define SER_RDSR				0x05		// Read Status Register
#define SER_WRSR				0x01		// Write Status Register
#define SER_READ				0x03		// Read Data from Memory Array
#define SER_WRITE				0x02		// Write Data to Memory Array

#define SER_SR_READY			1<<0		// Ready bit
#define SER_SR_WEN				1<<1		// Write Enable indicate 0:not write enabled 1:write enabled
#define SER_SR_BPx				3<<2		// Block Write Protect bits
#define SER_SR_WPEN				1<<7		// Write Protect Enable bit

#define SER_SE					0xD8		// Sector Erase
#define SER_BE					0xC7		// Bulk Erase
#define SER_DP					0xB9		// Deep Power-down
#define SER_RES					0xAB		// Release from Deep Power-down

#define	DELAY_R()				mdelay(10);
#define	DELAY_W()				mdelay(1);

struct NX_GPIO_RegisterSet 	 * pIO_SPI = (struct NX_GPIO_RegisterSet   *)(PHY_BASEADDR_GPIO_MODULE + OFFSET_OF_GPIO_MODULE * SPI0_GPIO_GROUP);
struct NX_GPIO_RegisterSet 	 * pIO_WP  = (struct NX_GPIO_RegisterSet   *)(PHY_BASEADDR_GPIO_MODULE + OFFSET_OF_GPIO_MODULE * WP_GPIO_GROUP);
struct NX_SSPSPI_RegisterSet * pSPIReg = (struct NX_SSPSPI_RegisterSet *)PHY_BASEADDR_SSPSPI_MODULE;

/*----------------------------------------------------------------------------*/
static void FlashSectorErase(U32 dwFlashAddr)
{
	volatile U8 __attribute__((__unused__))tmp;
	dwFlashAddr &= EEPROM_ERASE_MASK;

	DBGOUT("Sector Erase 0x%06X\n", dwFlashAddr);
	pSPIReg->DATA = SER_SE;

	pSPIReg->DATA = (U8)((dwFlashAddr >> 16) & 0xFF);
	pSPIReg->DATA = (U8)((dwFlashAddr >>  8) & 0xFF);
	pSPIReg->DATA = (U8)((dwFlashAddr >>  0) & 0xFF);

	pIO_SPI->GPIOxOUT &= ~(1UL<<SPI0_FRM_GPIO_NUM);		// output high
	pSPIReg->CONT0 |= 1<<11;	// spi start (cs will be low)

	while(!(pSPIReg->STATE & 1<<2));	// shift reg is empty
	while(!(pSPIReg->STATE & 1<<8));	// wait until tx buffer
	while(pSPIReg->STATE & 1<<0);	// wait until reception buffer is not empty

	pSPIReg->CONT0 &= ~(1<<11);			// SPI Stop
	pIO_SPI->GPIOxOUT |= 1UL<<SPI0_FRM_GPIO_NUM;		// output high

	tmp = pSPIReg->DATA;	// get dummy data ==> addr high
	tmp = pSPIReg->DATA;	// get dummy data ==> addr high
	tmp = pSPIReg->DATA;	// get dummy data ==> addr low
	DBGOUT("Sector Erase done\n");
	DELAY_W();
}

static void FlashPageProgram(U8*pBufferAddr, U32 dwFlashAddr, U32 dwDataSize)
{
	volatile U8 __attribute__((__unused__))tmp;
	U32 index=0;

	if(dwDataSize>0x100)
		dwDataSize = 0x100;

	DBGOUT("Page Program 0x%06X, Size:%X\n", dwFlashAddr, dwDataSize);

	while(!(pSPIReg->STATE & 1<<0))
		tmp = pSPIReg->DATA;

	pSPIReg->DATA = SER_WRITE;
	pSPIReg->DATA = (U8)((dwFlashAddr >> 16) & 0xFF);
	pSPIReg->DATA = (U8)((dwFlashAddr >>  8) & 0xFF);
	pSPIReg->DATA = (U8)((dwFlashAddr >>  0) & 0xFF);

	pIO_SPI->GPIOxOUT &= ~(1UL<<SPI0_FRM_GPIO_NUM);		// output high
	pSPIReg->CONT0 |= 1<<11;				// spi start (cs will be low)

	do{
		if(!(pSPIReg->STATE & 1<<3))	// check transmission buffer is full
			pSPIReg->DATA = pBufferAddr[index++];
		if(!(pSPIReg->STATE & 1<<0))
			tmp = pSPIReg->DATA;
	}while( index < dwDataSize);

	while(!(pSPIReg->STATE & 1<<0))
		tmp = pSPIReg->DATA;
	while(!(pSPIReg->STATE & 1<<2) || (!(pSPIReg->STATE & 1<<8)))		// shift reg is empty
	{
		if(!(pSPIReg->STATE & 1<<0))
			tmp = pSPIReg->DATA;
	}
//	while(!(pSPIReg->STATE & 1<<8));		// wait until tx buffer
	while(!(pSPIReg->STATE & 1<<0))
		tmp = pSPIReg->DATA;

	pSPIReg->CONT0 &= ~(1<<11);				// SPI Stop
	pIO_SPI->GPIOxOUT |= 1UL<<SPI0_FRM_GPIO_NUM;			// output high
	DBGOUT("Page Program done\n");
}

static void SPIFifoReset(void)
{
	U8 tmp = pSPIReg->CONT0;

//	DBGOUT("%s\n", __func__);
	pSPIReg->CONT0 = tmp | 1<<10;		// reset FIFO
	pSPIReg->CONT0 = tmp;
}

static void SetWriteEnable(U32 bEnb)
{
	volatile U8 __attribute__((__unused__))tmp;
	DBGOUT("%s\n", __func__);
	if(bEnb)
		pSPIReg->DATA = SER_WREN;
	else
		pSPIReg->DATA = SER_WRDI;

	pIO_SPI->GPIOxOUT &= ~(1UL<<SPI0_FRM_GPIO_NUM);		// output high
	pSPIReg->CONT0 |= 1<<11;								// spi start (cs will be low)

	while(!(pSPIReg->STATE & 1<<2));						// shift reg is empty
	while(!(pSPIReg->STATE & 1<<8));						// wait until tx buffer
	while(pSPIReg->STATE & 1<<0);							// wait until reception buffer is not empty

	pSPIReg->CONT0 &= ~(1<<11);								// SPI Stop
	pIO_SPI->GPIOxOUT |= 1UL<<SPI0_FRM_GPIO_NUM;			// output high
	tmp = pSPIReg->DATA;									// get dummy data
}

static U8 IsFlashReady(U8 status)
{
	volatile U8 tmp;
	U8  ret=0xFF;
	U32 dummycount = 1;

	DBGOUT("%s\n", __func__);

	pSPIReg->DATA = SER_RDSR;
	pIO_SPI->GPIOxOUT &= ~(1UL<<SPI0_FRM_GPIO_NUM);	// output high
	pSPIReg->CONT0 |= 1<<11;							// spi start (cs will be low)

	do{
		if(!(pSPIReg->STATE & 1<<3))	// check transmission buffer is full
			pSPIReg->DATA = SER_RDSR; 			// write dummy data for get status data
		if(!(pSPIReg->STATE & 1<<0))
		{
			ret = pSPIReg->DATA;
			if(dummycount) {
				dummycount--;
				ret = 0xFF;
			}
		}
	}while(ret& status);

	pSPIReg->CONT0 &= ~(1<<11);			// SPI Stop
	pIO_SPI->GPIOxOUT |= 1UL<<SPI0_FRM_GPIO_NUM;		// output high

	tmp = pSPIReg->CONT0;
	pSPIReg->CONT0 = tmp | 1<<10;		// reset FIFO
	pSPIReg->CONT0 = tmp;

	return ret;
}

/*----------------------------------------------------------------------------*/
void spi_init_f (void)
{
	pSPIReg->CLKENB = 0x1<<3;	// pclk mode on but not supply operation clock
	pSPIReg->CLKGEN = (8-1)<<5 | 0x1<<2;	// select clock source is pll1, 147MHz and supply clock is 147.5/8 = 18.4375MHz
	pSPIReg->CONT0 =
			0<<13 |			// Burst Receive Disable
			0<<12 |			// PIO mode
			0<<11 |			// SPI disable
			1<<10 |			// Reset FIFO
			0<< 9 |			// Internal Clock
		(8-1)<< 5 |			// data bit width
		(18-1)<<0;			// 18.4375MHz/18 = 1.0243MHz
	pSPIReg->CONT0 &= ~(1<<10);	// Reset FIFO Negate
	pSPIReg->CLKENB = 0x01<<3 | 0x1<<2;	// supply operation clock
	pSPIReg->CONT1 =
			0<<5 |			// no byte swap
			0<<4 |			// master mode
			1<<3 |			// 1: normal 0:inverse polarity
			0<<2 |			// Format A:0, B:1
			1<<0;			// SPI Mode

	pIO_WP->GPIOxOUT |= 1UL<<WP_GPIO_BITNUN;		// output high
	pIO_WP->GPIOxOUTENB |= 1UL<<WP_GPIO_BITNUN;	// output mode
	pIO_WP->GPIOxALTFN[WP_GPIO_BITNUN/16] &= ~(3UL<<((WP_GPIO_BITNUN%16)*2));	// gpio mode	gpio C 13 nWP

	pIO_SPI->GPIOxOUT |= 1UL<<SPI0_FRM_GPIO_NUM;		// output high
	pIO_SPI->GPIOxOUTENB |= 1UL<<SPI0_FRM_GPIO_NUM;	// output mode
	pIO_SPI->GPIOxALTFN[SPI0_FRM_GPIO_NUM/16] &= ~(3UL<<((SPI0_FRM_GPIO_NUM%16)*2));	// gpio mode	gpio A 28 nCS

  	DBGOUT("%s\n", __func__);
}

void spi_init_r (void)
{
	DBGOUT("%s\n", __func__);
	/* do nothing */
}

ssize_t spi_read  (uchar *addr, int alen, uchar *buffer, int len)
{
	volatile U8 __attribute__((__unused__))tmp;
	U32 dummycount, index=0;
	U8 *pBufferAddr = buffer;
	U32 dwFlashAddr = (U32)((addr[0]<<16) | (addr[1]<<8) | (addr[2]));
	U32 dwDataSize  = len;

	DBGOUT("\n read eeprom 0x%08x to 0x%08x length %d, (erase size %dbyte)\n",
		dwFlashAddr, (U32)buffer, len, EEPROM_ERASE_SIZE);

	pSPIReg->DATA = SER_READ;	// read command, Read Data from Memory Array

	dummycount = 4;
	pSPIReg->DATA = (U8)((dwFlashAddr >> 16) & 0xFF);	// start memory array address high byte
	pSPIReg->DATA = (U8)((dwFlashAddr >>  8) & 0xFF);		// start memory array address high byte
	pSPIReg->DATA = (U8)((dwFlashAddr >>  0) & 0xFF);		// start memory array address low byte

	pSPIReg->DATA = 0;			// send dummy data for receive read data.
	pIO_SPI->GPIOxOUT &= ~(1UL<<SPI0_FRM_GPIO_NUM);		// output low
	pSPIReg->CONT0 |= 1<<11;	// spi start (cs will be low)
	while( dwDataSize )
	{
		if(!(pSPIReg->STATE & 1<<0))	// check receive buffer is not empty
		{
			pSPIReg->DATA = 0;			// send dummy data for receive read data.
			if(dummycount != 0)
			{
				tmp = pSPIReg->DATA;
				dummycount--;
			}else
			{
				pBufferAddr[index] = (U8)pSPIReg->DATA;
				index++;
				dwDataSize--;
			}
		}
	}
	while(!(pSPIReg->STATE & 1<<2));		// shift reg is empty
	while(!(pSPIReg->STATE & 1<<8));		// wait until tx buffer
	do{
		tmp = pSPIReg->DATA;
	}while(!(pSPIReg->STATE & 1<<0));			// wait until reception buffer is not empty

	pSPIReg->CONT0 &= ~(1<<11);			// SPI Stop
	pIO_SPI->GPIOxOUT |= 1UL<<SPI0_FRM_GPIO_NUM;		// output high

	return len;
}

ssize_t spi_write (uchar *addr, int alen, uchar *buffer, int len)
{
	U32  TargetAddr = ((addr[0]<<16) | (addr[1]<<8) | (addr[2])) ;
	U8  *pDatBuffer = buffer;
	U8  *pTmpBuffer = NULL, *pWBuffer = NULL;
	U32	 FlashAddr = 0;
	U32  StartRest = 0, BlockCnt = 0;
	U32  StartOffs = 0, __attribute__((__unused__))EndOffs = 0;
	int  WriteSize = 0, StartBlock = 1;
	U8   tmp = 0;

	DBGOUT("\n Write 0x%08x to eeprom 0x%08x length %d, (erase size %dbyte)\n",
		(U32)buffer, TargetAddr, len, EEPROM_ERASE_SIZE);

	// page align
	if (0 == len) {
		printf("Fail, eeprom address is not page aligned (256:0x100)\n");
		return 0;
	}

	pTmpBuffer = malloc(EEPROM_ERASE_SIZE);
	if (NULL == pTmpBuffer) {
		printf("Fail, out of memory for eeprom buffer %dbyte\n", EEPROM_ERASE_SIZE);
		return 0;
	}

	WriteSize = len;
	FlashAddr = (TargetAddr & EEPROM_ERASE_MASK);
	BlockCnt  = ((TargetAddr& EEPROM_PAGE_MASK) + len) /
				(EEPROM_ERASE_SIZE) + (((TargetAddr + len) & EEPROM_PAGE_MASK) ? 1 : 0);

	StartOffs = (TargetAddr & EEPROM_PAGE_MASK);
	EndOffs   = (TargetAddr + len) & EEPROM_PAGE_MASK;
	StartRest = StartOffs     ? ((EEPROM_ERASE_SIZE) - StartOffs) : 0;
	pWBuffer  = pDatBuffer;

	DBGOUT("\n Write block count =%d, rest start=%d, len=%d \n", BlockCnt, StartRest, len);

	if (StartRest) {
		U8 offset[3] = { (FlashAddr >> 16), (FlashAddr >>  8), (FlashAddr & 0xFF) };
		int size = 0;

		if (BlockCnt > 1)
			WriteSize  += ((EEPROM_ERASE_SIZE) - StartRest), size = StartRest;
		else
			WriteSize   = (EEPROM_ERASE_SIZE), size = len;

		DBGOUT("\n SCopy 0x%08x to offset 0x%08x size %d\n", (U32)pDatBuffer, StartOffs, size);

		memset(pTmpBuffer, 0, EEPROM_ERASE_SIZE);
		DELAY_R();
		spi_read(offset, 0, pTmpBuffer, EEPROM_ERASE_SIZE);			// read block
		memcpy(pTmpBuffer + StartOffs, pDatBuffer, size);	// merge data

		pDatBuffer -= ((EEPROM_ERASE_SIZE) - StartRest);
		pWBuffer    = pTmpBuffer;
	}

	while (1)
	{
		// Erase Block
		if (0 == (FlashAddr & EEPROM_PAGE_MASK))
		{
			DBGOUT("\n Erase 0x%08x, BlockCnt %d\n", FlashAddr, BlockCnt);
			DELAY_R();
			if (1 == BlockCnt && (EEPROM_ERASE_SIZE) > WriteSize) {
				U8 offset[3] = { (FlashAddr >> 16), (FlashAddr >>  8), (FlashAddr & 0xFF) };
				DBGOUT("\n ECopy 0x%08x to offset 0x%08x size %d\n", (U32)pDatBuffer, 0, WriteSize);
				memset(pTmpBuffer, 0, EEPROM_ERASE_SIZE);
				spi_read(offset, 0, pTmpBuffer, EEPROM_ERASE_SIZE);	// read block
				memcpy(pTmpBuffer, pDatBuffer, WriteSize);	// merge data
				pWBuffer  = pTmpBuffer;
				WriteSize = (EEPROM_ERASE_SIZE);
			} else {
				if (! StartBlock)
					pWBuffer = pDatBuffer;
			}

			do {
				SetWriteEnable(1);
				tmp = IsFlashReady(SER_SR_READY);
			} while(!(!(tmp & SER_SR_READY) && (tmp & SER_SR_WEN)));

			FlashSectorErase(FlashAddr);

			do {
				tmp = IsFlashReady(SER_SR_READY);
			} while(tmp & SER_SR_READY);

			BlockCnt--;
			StartBlock = 0;
		}

		// Write Page
		SPIFifoReset();

		do {
			SetWriteEnable(1);
			tmp = IsFlashReady(SER_SR_READY);
		} while(!(!(tmp & SER_SR_READY) && (tmp & SER_SR_WEN)));

		SPIFifoReset();
		FlashPageProgram(pWBuffer, FlashAddr, 0x100);

		FlashAddr  += 256;
		pDatBuffer += 256;
		pWBuffer   += 256;	// write page size
		WriteSize  -= 256;
	//	DBGOUT("Page address 0x%08x, Write:%d\n", (U32)FlashAddr, WriteSize);
		if (0 >= WriteSize)
			break;
	}

	free(pTmpBuffer);
	DBGOUT("Write done ...\n");
	return len;
}
