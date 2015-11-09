////////////////////////////////////////////////////////////////////////////////
//
//	Copyright (C) 2009 Nexell Co. All Rights Reserved
//	Nexell Co. Proprietary & Confidential
//
//	Nexell informs that this code and information is provided "as is" base
//	and without warranty of any kind, either expressed or implied, including
//	but not limited to the implied warranties of merchantability and/or fitness
//	for a particular puporse.
//
//
//	Module		:
//	File		:
//	Description	:
//	Author		: Goofy
//	History		:
//
////////////////////////////////////////////////////////////////////////////////
#include <nx_type.h>
#include <nx_debug.h>
#include "BootHeader.h"

#include <common.h>
#include <command.h>

//------------------------------------------------------------------------------
#define	BASEADDR_SRAM				(0x18000000)

#define BASEADDR_BOOTSTATUS			(BASEADDR_SRAM+32)

//#define UDOWN_DEBUG
#ifdef UDOWN_DEBUG
#define NX_DEBUG_MSG(args...) printf(args)
#define NX_DEBUG_HEX(args...) printf("%x",args)
#define NX_DEBUG_DEC(args...) printf("%d",args)
#else
#define NX_DEBUG_MSG(args...) do{}while(0) 
#define NX_DEBUG_HEX(args...) do{}while(0)
#define NX_DEBUG_DEC(args...) do{}while(0)
#endif
//------------------------------------------------------------------------------
// USB Device Informations
//------------------------------------------------------------------------------
#define	HIGH_USB_VER				0x0200	// 2.0
#define	HIGH_MAX_PKT_SIZE_EP0		64
#define	HIGH_MAX_PKT_SIZE_EP1		512		// bulk
#define	HIGH_MAX_PKT_SIZE_EP2		512		// bulk

#define	FULL_USB_VER				0x0110	// 1.1
#define	FULL_MAX_PKT_SIZE_EP0		64		// Do not modify
#define	FULL_MAX_PKT_SIZE_EP1		64		// bulk
#define	FULL_MAX_PKT_SIZE_EP2		64		// bulk

#define	USB_VER					HIGH_USB_VER
#define	MAX_PKT_SIZE_EP0		HIGH_MAX_PKT_SIZE_EP0
#define	MAX_PKT_SIZE_EP1		HIGH_MAX_PKT_SIZE_EP1
#define	MAX_PKT_SIZE_EP2		HIGH_MAX_PKT_SIZE_EP2


#define VENDORID	0x2375		// (MagicEyes Vendor ID)
#define PRODUCTID	0x2120		// (C1000 Product ID)

enum DESCRIPTORTYPE
{
	DESCRIPTORTYPE_DEVICE			= 1,
	DESCRIPTORTYPE_CONFIGURATION	= 2,
	DESCRIPTORTYPE_STRING			= 3,
	DESCRIPTORTYPE_INTERFACE		= 4,
	DESCRIPTORTYPE_ENDPOINT			= 5,
};

#define	DEVICE_DESCRIPTOR_SIZE		(18)

//static const __align(2) U8 gs_DeviceDescriptorFS[DEVICE_DESCRIPTOR_SIZE] =
static const __attribute__ ((aligned (2))) U8 gs_DeviceDescriptorFS[DEVICE_DESCRIPTOR_SIZE] =
{
	18,							//	0 desc size
	(U8)(DESCRIPTORTYPE_DEVICE),//	1 desc type (DEVICE)
	(U8)(FULL_USB_VER % 0x100),	//	2 USB release
	(U8)(FULL_USB_VER / 0x100),	//	3 => 1.00
	0xFF,						//	4 class
	0xFF,						//	5 subclass
	0xFF,						//	6 protocol
	(U8)FULL_MAX_PKT_SIZE_EP0,	//	7 max pack size
	(U8)(VENDORID	% 0x100),	//	8 vendor ID LSB
	(U8)(VENDORID	/ 0x100),	//	9 vendor ID MSB
	(U8)(PRODUCTID % 0x100),	// 10 product ID LSB	(second product)
	(U8)(PRODUCTID / 0x100),	// 11 product ID MSB
	0x00,						// 12 device release LSB
	0x00,						// 13 device release MSB
	0x00,						// 14 manufacturer string desc index
	0x00,						// 15 product string desc index
	0x00,						// 16 serial num string desc index
	0x01						// 17 num of possible configurations
};

static const __attribute__ ((aligned (2))) U8 gs_DeviceDescriptorHS[DEVICE_DESCRIPTOR_SIZE] =
//static const __align(2) U8 gs_DeviceDescriptorHS[DEVICE_DESCRIPTOR_SIZE] =
{
	18,							//	0 desc size
	(U8)(DESCRIPTORTYPE_DEVICE),//	1 desc type (DEVICE)
	(U8)(HIGH_USB_VER % 0x100),	//	2 USB release
	(U8)(HIGH_USB_VER / 0x100),	//	3 => 1.00
	0xFF,						//	4 class
	0xFF,						//	5 subclass
	0xFF,						//	6 protocol
	(U8)HIGH_MAX_PKT_SIZE_EP0,	//	7 max pack size
	(U8)(VENDORID	% 0x100),	//	8 vendor ID LSB
	(U8)(VENDORID	/ 0x100),	//	9 vendor ID MSB
	(U8)(PRODUCTID % 0x100),	// 10 product ID LSB	(second product)
	(U8)(PRODUCTID / 0x100),	// 11 product ID MSB
	0x00,						// 12 device release LSB
	0x00,						// 13 device release MSB
	0x00,						// 14 manufacturer string desc index
	0x00,						// 15 product string desc index
	0x00,						// 16 serial num string desc index
	0x01						// 17 num of possible configurations
};

#define	CONFIG_DESCRIPTOR_SIZE		(9 + 9 + 7 + 7)

static const __attribute__ ((aligned (2)))  U8	gs_ConfigDescriptorFS[CONFIG_DESCRIPTOR_SIZE] =
//static const __align(2) U8	gs_ConfigDescriptorFS[CONFIG_DESCRIPTOR_SIZE] =
{
	//--------------------------------------------------------------------------
	// Configuration Descriptor
	0x09,								// [ 0] desc size
	(U8)(DESCRIPTORTYPE_CONFIGURATION),	// [ 1] desc type (CONFIGURATION)
	(U8)(CONFIG_DESCRIPTOR_SIZE % 0x100),// [ 2] total length of data returned LSB
	(U8)(CONFIG_DESCRIPTOR_SIZE / 0x100),// [ 3] total length of data returned MSB
	0x01,								// [ 4] num of interfaces
	0x01,								// [ 5] value to select config (1 for now)
	0x00,								// [ 6] index of string desc ( 0 for now)
	0x80,								// [ 7] bus powered
	25,									// [ 8] max power, 50mA for now

	//--------------------------------------------------------------------------
	// Interface Decriptor
	0x09,								// [ 0] desc size
	(U8)(DESCRIPTORTYPE_INTERFACE),		// [ 1] desc type (INTERFACE)
	0x00,								// [ 2] interface index.
	0x00,								// [ 3] value for alternate setting
	0x02,								// [ 4] bNumEndpoints (number endpoints used, excluding EP0)
	0xFF,								// [ 5]
	0xFF,								// [ 6]
	0xFF,								// [ 7]
	0x00,								// [ 8] string index,

	//--------------------------------------------------------------------------
	// Endpoint descriptor (EP 1 Bulk IN)
	0x07,								// [ 0] desc size
	(U8)(DESCRIPTORTYPE_ENDPOINT),		// [ 1] desc type (ENDPOINT)
	0x81,								// [ 2] endpoint address: endpoint 1, IN
	0x02,								// [ 3] endpoint attributes: Bulk
	(U8)(FULL_MAX_PKT_SIZE_EP1 % 0x100),// [ 4] max packet size LSB
	(U8)(FULL_MAX_PKT_SIZE_EP1 / 0x100),// [ 5] max packet size MSB
	0x00,								// [ 6] polling interval (4ms/bit=time,500ms)

	//--------------------------------------------------------------------------
	// Endpoint descriptor (EP 2 Bulk OUT)
	0x07,								// [ 0] desc size
	(U8)(DESCRIPTORTYPE_ENDPOINT),		// [ 1] desc type (ENDPOINT)
	0x02,								// [ 2] endpoint address: endpoint 2, OUT
	0x02,								// [ 3] endpoint attributes: Bulk
	(U8)(FULL_MAX_PKT_SIZE_EP2 % 0x100),// [ 4] max packet size LSB
	(U8)(FULL_MAX_PKT_SIZE_EP2 / 0x100),// [ 5] max packet size MSB
	0x00								// [ 6] polling interval (4ms/bit=time,500ms)
};

//static const __align(2) U8	gs_ConfigDescriptorHS[CONFIG_DESCRIPTOR_SIZE] =
static const __attribute__ ((aligned (2))) U8	gs_ConfigDescriptorHS[CONFIG_DESCRIPTOR_SIZE] =
{
	//--------------------------------------------------------------------------
	// Configuration Descriptor
	0x09,								// [ 0] desc size
	(U8)(DESCRIPTORTYPE_CONFIGURATION),	// [ 1] desc type (CONFIGURATION)
	(U8)(CONFIG_DESCRIPTOR_SIZE % 0x100),// [ 2] total length of data returned LSB
	(U8)(CONFIG_DESCRIPTOR_SIZE / 0x100),// [ 3] total length of data returned MSB
	0x01,								// [ 4] num of interfaces
	0x01,								// [ 5] value to select config (1 for now)
	0x00,								// [ 6] index of string desc ( 0 for now)
	0x80,								// [ 7] bus powered
	25,									// [ 8] max power, 50mA for now

	//--------------------------------------------------------------------------
	// Interface Decriptor
	0x09,								// [ 0] desc size
	(U8)(DESCRIPTORTYPE_INTERFACE),		// [ 1] desc type (INTERFACE)
	0x00,								// [ 2] interface index.
	0x00,								// [ 3] value for alternate setting
	0x02,								// [ 4] bNumEndpoints (number endpoints used, excluding EP0)
	0xFF,								// [ 5]
	0xFF,								// [ 6]
	0xFF,								// [ 7]
	0x00,								// [ 8] string index,

	//--------------------------------------------------------------------------
	// Endpoint descriptor (EP 1 Bulk IN)
	0x07,								// [ 0] desc size
	(U8)(DESCRIPTORTYPE_ENDPOINT),		// [ 1] desc type (ENDPOINT)
	0x81,								// [ 2] endpoint address: endpoint 1, IN
	0x02,								// [ 3] endpoint attributes: Bulk
	(U8)(HIGH_MAX_PKT_SIZE_EP1 % 0x100),// [ 4] max packet size LSB
	(U8)(HIGH_MAX_PKT_SIZE_EP1 / 0x100),// [ 5] max packet size MSB
	0x00,								// [ 6] polling interval (4ms/bit=time,500ms)

	//--------------------------------------------------------------------------
	// Endpoint descriptor (EP 2 Bulk OUT)
	0x07,								// [ 0] desc size
	(U8)(DESCRIPTORTYPE_ENDPOINT),		// [ 1] desc type (ENDPOINT)
	0x02,								// [ 2] endpoint address: endpoint 2, OUT
	0x02,								// [ 3] endpoint attributes: Bulk
	(U8)(HIGH_MAX_PKT_SIZE_EP2 % 0x100),// [ 4] max packet size LSB
	(U8)(HIGH_MAX_PKT_SIZE_EP2 / 0x100),// [ 5] max packet size MSB
	0x00								// [ 6] polling interval (4ms/bit=time,500ms)
};

//------------------------------------------------------------------------------
static void		USBInit( void );
static void		USBDeinit( void );

static void		ProcessingEtc( void );
static void		ProcessingEP0( void );
static void		ProcessingEP2( void );

typedef struct tag_USBBOOTSTATUS
{
	BOOTINFO	bootinfo;

	volatile CBOOL	bDownLoading;
	volatile CBOOL	bHeaderReceived;

	int		iRxHeaderSize;

	U8		CurConfig;
	U8		CurInterface;
	U8		CurSetting;
	U8		__Reserved;

	const U8	* DeviceDescriptor;
	const U8	* ConfigDescriptor;

	U32		BootHeader[512/4];
} USBBOOTSTATUS;

//------------------------------------------------------------------------------
//static	USBBOOTSTATUS * const pUSBBootStatus = (USBBOOTSTATUS *)BASEADDR_BOOTSTATUS;
static	USBBOOTSTATUS  * pUSBBootStatus;


unsigned int	iUSBBOOT( unsigned int option )
{
	// Not used
	option = option;

	pUSBBootStatus->DeviceDescriptor = gs_DeviceDescriptorHS;
	pUSBBootStatus->ConfigDescriptor = gs_ConfigDescriptorHS;

	pUSBBootStatus->bHeaderReceived = CFALSE;
	pUSBBootStatus->bDownLoading	= CTRUE;

	pUSBBootStatus->bootinfo.dwJumpAddr		= 0;
	//pUSBBootStatus->bootinfo.dwDownloadAddr = 0;
	pUSBBootStatus->bootinfo.iDownloadSize	= 0;
	pUSBBootStatus->iRxHeaderSize	= 0;

	pUSBBootStatus->CurConfig		= 0;
	pUSBBootStatus->CurInterface	= 0;
	pUSBBootStatus->CurSetting		= 0;

	
	//--------------------------------------------------------------------------
	USBInit();
	while( pUSBBootStatus->bDownLoading == CTRUE )
	{
	
		ProcessingEtc();
		ProcessingEP0();
		ProcessingEP2();
	}
	
	USBDeinit();

	//--------------------------------------------------------------------------
	#if defined(NX_DEBUG)
	{
		volatile int delay;

		NX_DEBUG_MSG("Jump To ");
		NX_DEBUG_HEX( pUSBBootStatus->bootinfo.dwJumpAddr );
		NX_DEBUG_MSG("\n");

		for( delay=0 ; delay<0xFFFFFF ; delay++ );	// Dummy delay
	}
	#endif

	return pUSBBootStatus->bootinfo.dwJumpAddr;
}

//------------------------------------------------------------------------------
#define		NX_UDC_BASEADDR		(0xC0018000)

//------------------------------------------------------------------------------
#define NX_UDC_EIR_EP2INT		(1<< 2)
#define NX_UDC_EIR_EP1INT		(1<< 1)
#define NX_UDC_EIR_EP0INT		(1<< 0)

//------------------------------------------------------------------------------
#define NX_UDC_TR_EUERR			(1<<13)
#define NX_UDC_TR_PERR			(1<<12)

//------------------------------------------------------------------------------
#define NX_UDC_SSR_BAERR		(1<<15)
#define NX_UDC_SSR_TMERR		(1<<14)
#define NX_UDC_SSR_BSERR		(1<<13)
#define NX_UDC_SSR_TCERR		(1<<12)
#define NX_UDC_SSR_DCERR		(1<<11)
#define NX_UDC_SSR_EOERR		(1<<10)
#define NX_UDC_SSR_VBUSOFF		(1<< 9)
#define NX_UDC_SSR_VBUSON		(1<< 8)
#define NX_UDC_SSR_TBM			(1<< 7)
#define	NX_UDC_SSR_HSP			(1<< 4)
#define	NX_UDC_SSR_SDE			(1<< 3)
#define	NX_UDC_SSR_HFRM			(1<< 2)
#define	NX_UDC_SSR_HFSUSP		(1<< 1)
#define	NX_UDC_SSR_HFRES		(1<< 0)

//------------------------------------------------------------------------------
#define NX_UDC_SCR_RRDE			(1<< 5)

//------------------------------------------------------------------------------
#define	NX_UDC_EP0SR_LWO		(1<< 6)
#define NX_UDC_EP0SR_SHT		(1<< 4)
#define	NX_UDC_EP0SR_TST		(1<< 1)
#define	NX_UDC_EP0SR_RSR		(1<< 0)

//------------------------------------------------------------------------------
#define	NX_UDC_EP0CR_SRE		(1<<15)
#define	NX_UDC_EP0CR_TTE		(1<< 3)
#define	NX_UDC_EP0CR_TTS		(1<< 2)
#define	NX_UDC_EP0CR_ESS		(1<< 1)
#define	NX_UDC_EP0CR_TZLS		(1<< 0)

//------------------------------------------------------------------------------
#define NX_UDC_ESR_FUDR			(1<<15)	// FIFO Underflow
#define NX_UDC_ESR_FOVF			(1<<14)	// FIFO Overflow
#define NX_UDC_ESR_FPID			(1<<11)	// First OUT Packet Interrupt Disable in OUT DMA Operation
#define NX_UDC_ESR_OSD			(1<<10)	// OUT Start DMA
#define NX_UDC_ESR_DTCZ			(1<< 9)	// DMA Total Counter Zero
#define NX_UDC_ESR_SPT			(1<< 8)	// Short Packet Received
#define NX_UDC_ESR_FFS			(1<< 6)	// FIFO Flushed
#define NX_UDC_ESR_FSC			(1<< 5)	// Functional Stall Condition
#define NX_UDC_ESR_LWO			(1<< 4)	// Last Word Odd
#define NX_UDC_ESR_PSIF			(1<< 2)	// Packet Status In FIFO
#define NX_UDC_ESR_TPS			(1<< 1)	// Tx Packet Success
#define NX_UDC_ESR_RPS			(1<< 0)	// Rx Packet Success

//------------------------------------------------------------------------------
#define	NX_UDC_ECR_ESS			(1<< 1)

//------------------------------------------------------------------------------
struct	NX_UDC_RegisterSet2
{
	volatile U16	IR;							// 0x00 : Index Register
	volatile U16	EIR;						// 0x02 : Endpoint Interrupt Pending Register
	volatile U16	EIER;						// 0x04 : Endpoint Interrupt Enable Register
	volatile U16	FAR;						// 0x06 : Frame Address Register
	volatile U16	FNR;						// 0x08 : Frame Number Register
	volatile U16	EDR;						// 0x0A : Endpoint Direction Register
	volatile U16	TR;							// 0x0C : Test Register
	volatile U16	SSR;						// 0x0E : System Status Register
	volatile U16	SCR;						// 0x10 : System Control Register
	volatile U16	EP0SR;						// 0x12 : Eendpoint0 Status Register
	volatile U16	EP0CR;						// 0x14 : Enddpoint0 Control Register
	volatile U16	ESR;						// 0x16 : Endpoint Status Register
	volatile U16	ECR;						// 0x18 : Endpoint Control Register
	volatile U16	BRCR;						// 0x1A : Byte Read Count Register
	volatile U16	BWCR;						// 0x1C : Byte Write Count Register
	volatile U16	MPR;						// 0x1E : Max Packet Register
	volatile U16	DCR;						// 0x20 : DMA Control Register
	volatile U16	DTCR;						// 0x22 : DMA Transfer Counter Register
	volatile U16	DFCR;						// 0x24 : DMA FIFO Counter Register
	volatile U16	DTTCR;						// 0x26 : DMA Total Transfer Counter Register 1
	volatile U16	DTTCR2;						// 0x28 : DMA Total Transfer Counter Register 1
	volatile U16	Reserved0[3];				// 0x2A ~ 0x2E : Reserved Region
	volatile U16	BR[4];						// 0x30 ~ 0x36 : Buffer Register
	volatile U16	Reserved1[(0x52-0x38)/2];	// 0x38 ~ 0x50 : Reserved Region
	volatile U16	PCR;						// 0x52 : PHY Control Register
	volatile U16	Reserved2[(0x840-0x54)/2];	// 0x54 ~ 0x83C :Reserved Region
	volatile U16	PHYCTRL0;					// 0x840 : PHY control 0
	volatile U16	VBUSINTENB;					// 0x842 : VBUS Interrupt Enable Register
	volatile U16	VBUSINTPEND;				// 0x844 : VBUS Interrupt Pending Register
	volatile U16	PHYCTRL1;					// 0x846 : PHY control 1
	volatile U16	PHYCTRL2;					// 0x848 : PHY control 2
	volatile U16	Reserved3[(0x8C0-0x84A)/2];	// 0x84A ~ 0x8BE : Reserved Region
	volatile U32	CLKENB;						// 0x8C0: Clock Enable Register
	volatile U32	CLKGEN;						// 0x8C4: Clock Generate Register
};

static struct NX_UDC_RegisterSet2 * const pUDCReg = (struct NX_UDC_RegisterSet2 *)NX_UDC_BASEADDR;

//------------------------------------------------------------------------------
static void		USBInit( void )
{
	//--------------------------------------------------------------------------
	// Initialize USB Device
	pUDCReg->CLKGEN = (0<<5) | (3<<2);				// CLKDIV = 0, CLKSRC = Ext Clock.
	pUDCReg->CLKENB = (1<<3) | (1<<2) | (3<<0);	// PCLKMODE=1, BCLKMODE=3, CLKGENENB=1

	//----------------------------------------------------------------------
	// Clear all pending bits & Disable all interrupts.
	pUDCReg->EIER	= 0;					// Disable all interrupts.

	pUDCReg->SCR	= (0<<9) | (1<<5)		// little endian
					| (0<<6) | (1<<3);		// Disable Speed Detection Control

	//pUDCReg->SCR	= 0x5163;

	pUDCReg->EDR	= (1<<1)				// EP1 <= TX
					| (0<<2);				// EP2 <= RX

	//----------------------------------------------------------------------
	// Endpoint 0
	pUDCReg->IR		= 0;
	pUDCReg->MPR	= MAX_PKT_SIZE_EP0;	// Set max packet size.
	pUDCReg->EP0CR	= 0;
	pUDCReg->EP0SR	= NX_UDC_EP0SR_SHT | NX_UDC_EP0SR_TST	| NX_UDC_EP0SR_RSR;

	//----------------------------------------------------------------------
	// Endpoint 1
	pUDCReg->IR		= 1;
	pUDCReg->MPR	= MAX_PKT_SIZE_EP1;	// Set max packet size.
	pUDCReg->DCR	= 0;				// Disable DMA mode
	pUDCReg->ECR	= (1<<9)			// transaction number per microsecond = 1
					| (0<<8) | (0<<0)	// bulk transfer mode
					| (1<<7)			// Dual FIFO mode
					| (0);
	pUDCReg->ESR	= 0xC762;			// Clear all status

	//----------------------------------------------------------------------
	// Endpoint 2
	pUDCReg->IR		= 2;
	pUDCReg->MPR	= MAX_PKT_SIZE_EP2;	// Set max packet size.
	pUDCReg->DCR	= 0;				// Disable DMA mode
	pUDCReg->ECR	= (1<<9)			// transaction number per microsecond = 1
					| (0<<8) | (0<<0)	// bulk transfer mode
					| (1<<7)			// Dual FIFO mode
					| (0);
	pUDCReg->ESR	= 0xC762;			// Clear all status

	//----------------------------------------------------------------------
	pUDCReg->TR		= NX_UDC_TR_EUERR | NX_UDC_TR_PERR;	// clear all errors.
	pUDCReg->SSR	= 0xFFFF;								// clear all status.

	pUDCReg->EIR	= 0xF;				// clear all pendings.

	// PHY Power-up
	pUDCReg->PHYCTRL0	= 0x0037;
	pUDCReg->PHYCTRL2	= 0x0038;
}

//------------------------------------------------------------------------------
static void		USBDeinit( void )
{
	// PHY Power-down
	pUDCReg->PHYCTRL0	= 0x013C;
	pUDCReg->PHYCTRL2	= 0x0030;

	pUDCReg->IR		= 1;
	pUDCReg->ECR	|= (1<<6);			// Flush FIFO
	while( pUDCReg->ECR & (1<<6) );
	pUDCReg->ESR	= 0xC763;			// Clear all status

	pUDCReg->IR		= 2;
	pUDCReg->ECR	|= (1<<6);			// Flush FIFO
	while( pUDCReg->ECR & (1<<6) );
	pUDCReg->ESR	= 0xC763;			// Clear all status

	pUDCReg->IR		= 0;
	pUDCReg->EP0SR	= NX_UDC_EP0SR_SHT | NX_UDC_EP0SR_TST	| NX_UDC_EP0SR_RSR;

	pUDCReg->TR		= NX_UDC_TR_EUERR | NX_UDC_TR_PERR;	// clear all errors.
	pUDCReg->SSR	= 0xFFFF;								// clear all status.
	pUDCReg->EIR	= 0xF;				// clear all pendings.

	pUDCReg->CLKENB = (0<<3) | (0<<2) | (0<<0);	// PCLKMODE=0, BCLKMODE=0, CLKGENENB=0
}

//------------------------------------------------------------------------------
static void		ProcessingEtc( void )
{
	U32 status;

	status = pUDCReg->SSR;	
	pUDCReg->SSR = status;

	#if (0) && defined(NX_DEBUG)
	if( NX_UDC_SSR_HFRES & status )
		NX_DEBUG_MSG(" - RESET\n");

	if( NX_UDC_SSR_HFRM & status )
		NX_DEBUG_MSG(" - RESUME\n");

	if( NX_UDC_SSR_HFSUSP & status )
		NX_DEBUG_MSG(" - SUSPEND\n");

	if( NX_UDC_SSR_VBUSON & status )
		NX_DEBUG_MSG(" - CONNECT\n");

	if( NX_UDC_SSR_VBUSOFF & status )
		NX_DEBUG_MSG(" - DISCONNECT\n");

	if( NX_UDC_SSR_BAERR & status )
		NX_DEBUG_MSG(" - ERROR : Byte Align error!\n");

	if( NX_UDC_SSR_TMERR & status )
		NX_DEBUG_MSG(" - ERROR : Timeout error!\n");

	if( NX_UDC_SSR_BSERR & status )
		NX_DEBUG_MSG(" - ERROR : BitStuff error!\n");

	if( NX_UDC_SSR_TCERR & status )
		NX_DEBUG_MSG(" - ERROR : Token CRC error!\n");

	if( NX_UDC_SSR_DCERR & status )
		NX_DEBUG_MSG(" - ERROR : Data CRC error!\n");

	if( NX_UDC_SSR_EOERR & status )
		NX_DEBUG_MSG(" - ERROR : EB Overrun error!\n");

	if( NX_UDC_SSR_TBM & status )
		NX_DEBUG_MSG(" - ERROR : Toggle Bit Mismatch error!\n");
	#endif	// NX_DEBUG

	if( NX_UDC_SSR_SDE & status )
	{
		if( NX_UDC_SSR_HSP & status)
		{
			NX_DEBUG_MSG(" - High Speed Mode\n");

			pUSBBootStatus->DeviceDescriptor = gs_DeviceDescriptorHS;
			pUSBBootStatus->ConfigDescriptor = gs_ConfigDescriptorHS;

			pUDCReg->IR		= 0;
			pUDCReg->MPR	= HIGH_MAX_PKT_SIZE_EP0;
			pUDCReg->IR		= 1;
			pUDCReg->MPR	= HIGH_MAX_PKT_SIZE_EP1;
			pUDCReg->IR		= 2;
			pUDCReg->MPR	= HIGH_MAX_PKT_SIZE_EP2;
		}
		else
		{
			NX_DEBUG_MSG(" - Full Speed Mode\n");

			pUSBBootStatus->DeviceDescriptor = gs_DeviceDescriptorFS;
			pUSBBootStatus->ConfigDescriptor = gs_ConfigDescriptorFS;

			pUDCReg->IR		= 0;
			pUDCReg->MPR	= FULL_MAX_PKT_SIZE_EP0;
			pUDCReg->IR		= 1;
			pUDCReg->MPR	= FULL_MAX_PKT_SIZE_EP1;
			pUDCReg->IR		= 2;
			pUDCReg->MPR	= FULL_MAX_PKT_SIZE_EP2;
		}
	}

	status = pUDCReg->TR;
	pUDCReg->TR = NX_UDC_TR_EUERR | NX_UDC_TR_PERR;

	#if defined(NX_DEBUG)
	if( NX_UDC_TR_EUERR & status )
		NX_DEBUG_MSG(" - ERROR : EB Underrun error!\n");

	#if 0
	if( NX_UDC_TR_PERR & status )
		NX_DEBUG_MSG(" - ERROR : PID error!\n");
	#endif
	#endif // NX_DEBUG
}

//------------------------------------------------------------------------------
static	void	ProcessingEP0( void )
{
	const U16* pTxData;
	U32	dwTxSize;
	U32 status;

	struct
	{
		unsigned char bmRequest, bRequest;
		unsigned short wValue, wIndex, wLength;
	} SetupPacket;
	U16 *pSetupPacket = (U16 *)&SetupPacket;

	int i;

	status = (U32)pUDCReg->EP0SR;		// Get EP0 Status

	//--------------------------------------------------------------------------
	// Stall
	if( NX_UDC_EP0SR_SHT & status )
	{
		//NX_DEBUG_MSG("UDC : EP0 Stall Tx !\n");
		pUDCReg->EP0CR &= ~NX_UDC_EP0CR_ESS;	// Clear Stall Enable
		pUDCReg->EP0SR = NX_UDC_EP0SR_SHT;	// Clear Stall pending
	}

	//--------------------------------------------------------------------------
	// TX
	if( NX_UDC_EP0SR_TST & status )
	{
		pUDCReg->EP0SR = NX_UDC_EP0SR_TST;	// Clear Tx pending
	}

	//--------------------------------------------------------------------------
	// RX
	if( NX_UDC_EP0SR_RSR & status )
	{
		pUDCReg->IR = 0;

		NX_ASSERT( pUDCReg->BRCR == 4 );	// must be 8 bytes in the FIFO.
		pSetupPacket[0] = pUDCReg->BR[0];
		pSetupPacket[1] = pUDCReg->BR[0];
		pSetupPacket[2] = pUDCReg->BR[0];
		pSetupPacket[3] = pUDCReg->BR[0];

		// Decode and execute the command. We support two sets of commands, vendor
		// specific (modem control) and chapter 9 standard commands.
		if( SetupPacket.bmRequest & 0x60 )	// vendor or class command
		{
			//return;
		}
		else
		{
			#if (0) && defined(NX_DEBUG)
			{
			const char	*szRequestType[] = {"Standard", "USB Quarified", "Manufacture" };
			const char	*szRecipient[] = { "Device", "Interface", "Endpoint", "Ohter" };
			const char	*szRequest[]	= {"Get Status", "Clear Feature", "-", "Set Feature",
										"-", "Set Address", "Get Descripture", "Set Descripture",
										"Get Configuration", "Set Configuration", "Get Interface", "Get Interface",
										"Sync Frame"
										};

			NX_DEBUG_MSG("\n-------------------[");
			NX_DEBUG_MSG( (pReg->SSR & NX_UDC_SSR_HSP) ? "High Speed" : "Full Speed" );
			NX_DEBUG_MSG("]------------------------\n");

			NX_DEBUG_MSG("bmRequstType	: ");
			NX_DEBUG_HEX( SetupPacket.bmRequest );
			NX_DEBUG_MSG("\n");

			NX_DEBUG_MSG(" - Direction	: ");
			NX_DEBUG_MSG( (SetupPacket.bmRequest & (1<<7)) ? "IN" : "OUT" );
			NX_DEBUG_MSG("\n");

			NX_DEBUG_MSG(" - Request Type : ");
			NX_DEBUG_HEX( (SetupPacket.bmRequest >> 5) & 0x03 );
			NX_DEBUG_MSG("(");
			NX_DEBUG_MSG( szRequestType[ (SetupPacket.bmRequest >> 5) & 0x03 ] );
			NX_DEBUG_MSG(")\n");

			NX_DEBUG_MSG(" - Recipient bit: ");
			NX_DEBUG_HEX( SetupPacket.bmRequest&0x0F );
			NX_DEBUG_MSG("(");
			NX_DEBUG_MSG( szRecipient[SetupPacket.bmRequest&0x0F] );
			NX_DEBUG_MSG(")\n");

			NX_DEBUG_MSG("bRequest : ");
			NX_DEBUG_HEX( SetupPacket.bRequest );
			NX_DEBUG_MSG("(");
			NX_DEBUG_MSG( szRequest[SetupPacket.bRequest] );
			NX_DEBUG_MSG(")\n");

			NX_DEBUG_MSG("wValue	: ");
			NX_DEBUG_HEX( SetupPacket.wValue );
			NX_DEBUG_MSG("\n");

			NX_DEBUG_MSG("wIndex	: ");
			NX_DEBUG_HEX( SetupPacket.wIndex );
			NX_DEBUG_MSG("\n");
			NX_DEBUG_MSG("wLength : ");
			NX_DEBUG_HEX( SetupPacket.wLength );
			NX_DEBUG_MSG("\n");
			}
			#endif

			// Standard	Request	Codes (in bRequest)
			#define	GET_STATUS				0x00	//
			#define	CLEAR_FEATURE			0x01	//
			#define	SET_FEATURE				0x03	//
			#define	SET_ADDRESS				0x05	//
			#define	GET_DESCRIPTOR			0x06	//
			#define	SET_DESCRIPTOR			0x07	//
			#define	GET_CONFIGURATION		0x08	//
			#define	SET_CONFIGURATION		0x09	//
			#define	GET_INTERFACE			0x0a	//
			#define	SET_INTERFACE			0x0b	//
			#define	SYNCH_FRAME				0x0c	//

			// standard chapter 9 commands
			switch (SetupPacket.bRequest)
			{
			case GET_STATUS:
				pUDCReg->EP0SR = NX_UDC_EP0SR_RSR;	// Clear Rx pending to send ACK to Host

				pUDCReg->BWCR = 2;
				pUDCReg->BR[0] = 0;
				return;

			case CLEAR_FEATURE:
			case SET_FEATURE:
			case SET_ADDRESS:		// LF1000 USB Device automatically setting Device Address
				break;

			case GET_DESCRIPTOR:
				switch ((U8)(SetupPacket.wValue>>8))
				{
				case DESCRIPTORTYPE_DEVICE:
					pTxData	= (const U16 *)pUSBBootStatus->DeviceDescriptor;
					dwTxSize		= ( DEVICE_DESCRIPTOR_SIZE > SetupPacket.wLength ) ? SetupPacket.wLength : DEVICE_DESCRIPTOR_SIZE;
					break;

				case DESCRIPTORTYPE_CONFIGURATION:
					pTxData	= (const U16 *)pUSBBootStatus->ConfigDescriptor;
					dwTxSize		= ( CONFIG_DESCRIPTOR_SIZE > SetupPacket.wLength ) ? SetupPacket.wLength : CONFIG_DESCRIPTOR_SIZE;
					break;

				default:
					//NX_DEBUG_MSG("..........STALL.\n");
					pUDCReg->EP0CR |= NX_UDC_EP0CR_ESS;	// Send Stall to Host
					return;
				}

				pUDCReg->EP0SR = NX_UDC_EP0SR_RSR;	// Clear Rx pending to send ACK to Host

				// Assume dwTxLeft must be less than MAX_PKT_SIZE_EP0
				NX_ASSERT( dwTxSize < MAX_PKT_SIZE_EP0 );

				// Send reply to Host
				pUDCReg->BWCR = dwTxSize;
				for( i=0 ; i<dwTxSize/2 ; i++ )		pUDCReg->BR[0] = pTxData[i];
				if( dwTxSize & 1 )					pUDCReg->BR[0] = (pTxData[i] & 0x00FF);

				return;

			case GET_CONFIGURATION:
				pUDCReg->EP0SR = NX_UDC_EP0SR_RSR;	// Clear Rx pending to send ACK to Host

				pUDCReg->BWCR	= 1;
				pUDCReg->BR[0]	= (U16)pUSBBootStatus->CurConfig;
				return;

			case GET_INTERFACE:
				pUDCReg->EP0SR = NX_UDC_EP0SR_RSR;	// Clear Rx pending to send ACK to Host

				pUDCReg->BWCR	= 1;
				pUDCReg->BR[0]	= (U16)pUSBBootStatus->CurInterface;
				return;

			case SET_CONFIGURATION:
				pUSBBootStatus->CurConfig		= (U8)SetupPacket.wValue;
				break;

			case SET_DESCRIPTOR:
				break;

			case SET_INTERFACE:
				pUSBBootStatus->CurInterface	= (U8)SetupPacket.wIndex;
				pUSBBootStatus->CurSetting		= (U8)SetupPacket.wValue;
				break;

			default:
				pUDCReg->EP0CR |= NX_UDC_EP0CR_ESS;	// Send Stall to Host
				return;
				//break;
			}
		}

		pUDCReg->EP0SR = NX_UDC_EP0SR_RSR;	// Clear Rx pending to send ACK to Host
	}
}

//-----------------------------------------------------------------------------
static U32	ReadPacketEP2( U16	*pwBuffer )
{
	U32	dwRxSize, dwRxByteSize;
	int		i;

	NX_ASSERT( 0 == ((U32)pwBuffer & 1) );

	dwRxSize = pUDCReg->BRCR;

	dwRxByteSize = dwRxSize << 1;
	if( NX_UDC_ESR_LWO & pUDCReg->ESR )		dwRxByteSize--;

	for( i=0 ; i<dwRxSize ; i++ )
	{
		*pwBuffer++ = pUDCReg->BR[2];
	}

	return dwRxByteSize;
}

//------------------------------------------------------------------------------
static void		ProcessingEP2( void )
{
	U32 *pdwBuffer;
	U32 status;
	int	iRxSize;

	pUDCReg->IR = 2;

	status = pUDCReg->ESR;		// Get status

	#if defined(NX_DEBUG)
	if( NX_UDC_ESR_DTCZ & status )
		NX_DEBUG_MSG(" - ERROR : UDC EP2 - DMA Total Count Zero !\n");

	if( NX_UDC_ESR_FUDR & status )
		NX_DEBUG_MSG(" - ERROR : UDC EP2 - Fifo Underflow !\n");

	if( NX_UDC_ESR_FOVF & status )
		NX_DEBUG_MSG(" - ERROR : UDC EP2 - Fifo Overflow !\n");

	if( NX_UDC_ESR_SPT & status )
		NX_DEBUG_MSG(" - ERROR : UDC EP2 - Short Packet Received !\n");

	if( NX_UDC_ESR_OSD & status )
		NX_DEBUG_MSG(" - ERROR : UDC EP2 - OUT Start DMA !\n");

	if( NX_UDC_ESR_FFS & status )
		NX_DEBUG_MSG(" - ERROR : UDC EP2 - FIFO Flushed !\n");
	#endif

	if( NX_UDC_ESR_FSC & status )
	{
		//NX_DEBUG_MSG("UDC EP2 : Function Stall Condition !\n");
		pUDCReg->ESR = NX_UDC_ESR_FSC;
		pUDCReg->ECR &= ~NX_UDC_ECR_ESS;
	}

	if( NX_UDC_ESR_RPS & status )
	{
		NX_DEBUG_MSG("ESR : ");
		NX_DEBUG_HEX(status);
		NX_DEBUG_MSG(" ->	");

		if( CTRUE != pUSBBootStatus->bHeaderReceived )
		{
			pdwBuffer = pUSBBootStatus->BootHeader;

			iRxSize = ReadPacketEP2( (U16*)&pdwBuffer[pUSBBootStatus->iRxHeaderSize/4] );

			NX_DEBUG_MSG("Header Packet Size = ");
			NX_DEBUG_DEC( iRxSize );
			NX_DEBUG_MSG(", ");
			NX_DEBUG_DEC( pUSBBootStatus->iRxHeaderSize );
			NX_DEBUG_MSG("\n");

			if( (iRxSize & 3) == 0 )
			{
				pUSBBootStatus->iRxHeaderSize += iRxSize;
			}
			else 
			{
				NX_DEBUG_MSG("ERROR : Header Packet Size must be aligned on 32-bits.\n");
				pUDCReg->ECR |= NX_UDC_ECR_ESS;
			}

			if( 512 <= pUSBBootStatus->iRxHeaderSize )
			{
				if( CTRUE == ProcessBootHeader( pdwBuffer, &pUSBBootStatus->bootinfo ) )
				{
					pUSBBootStatus->bHeaderReceived = CTRUE;
				}
				else
				{
					pUSBBootStatus->iRxHeaderSize = 0;
					pUDCReg->ECR |= NX_UDC_ECR_ESS;
				}
			}
		}
		else
		{
			NX_ASSERT( (pUSBBootStatus->bootinfo.iDownloadSize) > 0 );
			NX_ASSERT( 0 == (pUSBBootStatus->bootinfo.dwDownloadAddr & 1) );
			iRxSize = ReadPacketEP2( (U16*)(pUSBBootStatus->bootinfo.dwDownloadAddr) );

			#if (1)
			NX_DEBUG_MSG("Bin Packet Size = ");
			NX_DEBUG_DEC( iRxSize );
			NX_DEBUG_MSG(" => ");
			NX_DEBUG_HEX( pUSBBootStatus->bootinfo.dwDownloadAddr );
			NX_DEBUG_MSG(", ");
			NX_DEBUG_DEC( pUSBBootStatus->bootinfo.iDownloadSize );
			NX_DEBUG_MSG("\n");
			#endif

			pUSBBootStatus->bootinfo.dwDownloadAddr	+= iRxSize;
			pUSBBootStatus->bootinfo.iDownloadSize	-= iRxSize;

			if( pUSBBootStatus->bootinfo.iDownloadSize <= 0 )
			{
				//NX_DEBUG_MSG("Download completed!\n" );
				printf("Download completed!\n" );

				pUSBBootStatus->bDownLoading	= CFALSE;
				pUSBBootStatus->bHeaderReceived = CFALSE;
			}
		}
	}

	pUDCReg->ESR = status & ( NX_UDC_ESR_FUDR | NX_UDC_ESR_FOVF | NX_UDC_ESR_FPID	| NX_UDC_ESR_OSD 
							| NX_UDC_ESR_DTCZ | NX_UDC_ESR_SPT	| NX_UDC_ESR_FFS	| NX_UDC_ESR_FSC
							| NX_UDC_ESR_TPS );
}

int nxp_usbdn_size = 0; // add falinux

int do_usbdown(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	unsigned int addr;
	USBBOOTSTATUS status;
	
	pUSBBootStatus = &status;
	addr = simple_strtoul(argv[1],NULL,16);

	if(addr < 0x80000000)
		goto usage;

	printf("Download Address %x\n",addr);
	pUSBBootStatus->bootinfo.dwDownloadAddr = addr;
	iUSBBOOT(1);

	// add falinux
	nxp_usbdn_size = pUSBBootStatus->bootinfo.dwDownloadAddr - addr;

	return 0;

usage:
	cmd_usage(cmdtp);
	return 1;	
}




U_BOOT_CMD(
	udown,CONFIG_SYS_MAXARGS, 1, do_usbdown,
	"Download USB",
	"udown addr(hex) "
);








