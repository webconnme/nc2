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
#include <common.h>
#include <nx_type.h>
#include <nx_debug.h>
#include "BootHeader.h"

#include "../module/nx_mcud.h"
#include "../module/nx_clkpwr.h"

//------------------------------------------------------------------------------
#define REG_CLKPWR_PWRMODE_CHGPLL		(1U<<15)

//#define UBOOTHEADER_DEBUG 
#ifdef UBOOTHEADER_DEBUG 
#define NX_DEBUG_MSG(args...) printf(args)
#define NX_DEBUG_HEX(args...) printf("%x",args)
#define NX_DEBUG_DEC(args...) printf("%d",args)
#else
#define NX_DEBUG_MSG(args...) do{}while(0) 
#define NX_DEBUG_HEX(args...) do{}while(0)
#define NX_DEBUG_DEC(args...) do{}while(0)
#endif

static struct	NX_MCUD_RegisterSet * const pMCUD = (struct	NX_MCUD_RegisterSet *)PHY_BASEADDR_MCUD_MODULE;
static struct	NX_CLKPWR_RegisterSet * const pCLKPWR = (struct	NX_CLKPWR_RegisterSet *)PHY_BASEADDR_CLKPWR_MODULE;
//------------------------------------------------------------------------------
void	MemoryInitWithRstCfg( unsigned int ResetConfig )
{
	volatile U32 temp;

	U32 DDRType		= (ResetConfig>>19) & 3;
	U32 DDRSize		= (ResetConfig>>21) & 3;
	U32 DDRBW		= (ResetConfig>>23) & 1;

	pMCUD->MEMTIME0	= (0x11U<<24)	// [31:24] : tRFC
					| (0x05U<<20)	// [23:20] : ?
					| (0x09U<<12)	// [19:12] : tRAS
					| (0x02U<< 4)	// [ 7: 4] : tRP
					| (0x02U<< 0)	// [ 3: 0] : tRC
					| (0);

	pMCUD->MEMTIME1	= (0x5U<<28)	// [31:28] : tWTR (write to Read time)+ WL
					| (0x2U<<24)	// [27:24] : tWR
					| (0x1U<<20)	// [23:20] : tRTP (Read to Pre time)
					| (0x2U<<16)	// [19:16] : tMRD
					| (0x0100<<0)	// [15: 0] : REFP
					| (0);

	switch( DDRType )
	{
	//--------------------------------------------------------------------------
	// DDR2
	case 0 :
		pMCUD->MEMCFG	= (0U<<27)				// [27]		: nDQSEnb(Only DDR2)
						| (3U<<24)				// [26:24]	: Delay Read Latency(DDR2PHY Latency)
						| (5U<<21)				// [23:21]	: Cas Latency
						| (0U<<18)				// [20:18]	: Add Latency
						| (1U<<16)				// [17:16]	: Rtt Config
						| (0U<<10)				// [10]		: DIC Config
						| (0U<< 9)				// [ 9]		: DLL Config
						| (2U<< 7)				// [ 8: 7]	: Memory Type (00: Reserved, 01: DDR, 10: DDR2, 11: MDDR)
						| (1U<< 5)				// [ 6: 5]	: Memory Bank BUS Width (1:16-bit)
						| ((DDRBW	+1)<< 3)	// [ 4: 3]	: SD-DRAM Data bit width (2:8-bit, 3:16-bit)
						| ((DDRSize +2)<< 0)	// [ 2: 0]	: DDR Size (2:256Mbit, 3:512Mbit, 4:1Gbit, others:Reserved)
						| (0);
		pMCUD->PHYMODE	= 0;

		// PHYZQENB
		pMCUD->PHYZQCTRL	= 1;
		while( 1 )
		{
			temp = (pMCUD->PHYZQCTRL>>2) & 3;
			if( 1 == temp )		// ZQ END Check
			{
				break;
			}
			else if( 2 == temp )	// ZQ Error Check
			{
				// Error, do manual
				pMCUD->PHYZQCTRL	= 0;
				pMCUD->PHYZQCTRL	= 2;
				pMCUD->PHYZQFORCE	= 0x15;
				break;
			}
		}
		pMCUD->PHYUPDATE	= 1;

		break;

	//--------------------------------------------------------------------------
	case 1 :	// DDR
	case 2 :	// Mobile DDR with DLL Lock
	case 3 :	// Mobile DDR without DLL Lock
	default:
		temp			= (0U<<27)				// [27]		: nDQSEnb(Only DDR2)
						| (0U<<24)				// [26:24]	: Delay Read Latency(DDR2PHY Latency)
						| (3U<<21)				// [23:21]	: Cas Latency
						| (0U<<18)				// [20:18]	: Add Latency
						| (0U<<16)				// [17:16]	: Rtt Config
						| (0U<<10)				// [10]		: DIC Config
						| (0U<< 9)				// [ 9]		: DLL Config
						| (1U<< 7)				// [ 8: 7]	: Memory Type (00: Reserved, 01: DDR, 10: DDR2, 11: MDDR)
						| (1U<< 5)				// [ 6: 5]	: Memory Bank BUS Width (1:16-bit)
						| ((DDRBW	+1)<< 3)	// [ 4: 3]	: SD-DRAM Data bit width (2:8-bit, 3:16-bit)
						| ((DDRSize +2)<< 0)	// [ 2: 0]	: DDR Size (2:256Mbit, 3:512Mbit, 4:1Gbit, others:Reserved)
						| (0);

		temp |=	(DDRType & 2) << 7;
		pMCUD->MEMCFG = temp;

		pMCUD->PHYMODE	= 2;
		pMCUD->PHYTERMCTRL	= 0x71;
		break;
	}

	//--------------------------------------------------------------------------
	if( DDRType == 3 )	// MobileDDR without PHYDLOCK
	{
		temp = (ResetConfig>> 1) & 0x7F;	// Just use for MobileDDR
		pMCUD->PHYDELAYCTRL	= (temp<<7) | (temp);

		pMCUD->PHYDLLCTRL1	= 0;
		pMCUD->PHYDLLCTRL0	= 0xF009D9DE;
		pMCUD->PHYDLLFORCE	= 0x7F;
		pMCUD->PHYDLLCTRL1	= 1;
	}
	else
	{
		pMCUD->PHYDLLCTRL1	= 3;
	}

	//--------------------------------------------------------------------------
	pMCUD->MEMCTRL	= 0x00008000;

	//--------------------------------------------------------------------------
	// PLL Change for Mobile DDR without DLL Lock.
	if( DDRType == 3 )
	{
		pCLKPWR->CLKMODEREG1 = (1U<<12)	// CLKDIVPCLK = BCLK / 2 = 33 Mhz
							| (0U<< 8)	// CLKDIVBCLK = MCLK / 1 = 66 Mhz
							| (0U<< 4)	// CLKSELMCLK = PLL0
							| (7U<< 0)	// CLKDIVMCLK = PLL0 / 8 = 528 Mhz / 8 = 66 Mhz
							| (0);

		pCLKPWR->PWRMODE |= REG_CLKPWR_PWRMODE_CHGPLL;
		while( pCLKPWR->PWRMODE & REG_CLKPWR_PWRMODE_CHGPLL );
	}
}

//------------------------------------------------------------------------------
typedef struct tag_MEMORYCONFIG
{
	U32 OPTION		;	// Option
						// [0] : Update DRAM configuration
						// [1] : PHYZQENB : valid only when OPTION[0] == 1
						// [2] : PHYDLOCK : valid only when OPTION[0] == 1
						// [3] : Update Fast Channel Arbiter
						// [4] : Update Slow Channel Arbiter
						// [5] : Change PLL
						// [31:6] : Reserved for future use. must be 0
	U32	DELAY		;	// Delay counter

	// MCU-D Registers
	U32 CONFIG		;	// 0x00
	U32 TIME0		;	// 0x04
	U32 ENABLE		;	// 0x08
	U32 TIME1		;	// 0x10
	U32 FASTCH[3]	;	// 0x20, 0x24, 0x28
	U32 SLOWCH[3]	;	// 0x2C, 0x30, 0x34
	U32 DQSOFFSET	;	// 0x94
	U32 PHYDLL		;	// 0x98
	U32 PHYMODE		;	// 0x9C
	U32 DLLLOCKSTART;	// 0xA0
	U32 DLLLOCKFORCE;	// 0xA4
	U32 ZQSTART		;	// 0xAC
	U32 ZQFORCE		;	// 0xB0
	U32 ZQENB		;	// 0xB4
	U32 UPDATE		;	// 0xB8

	// Clock & Power Management Registers
	U32	CLKMODE[2]	;
	U32	PLLSET[2]	;
} MEMORYCONFIG;

#define	UPDATE_MCUD			(1<<0)		// [0] : Update DRAM configuration
#define ENABLE_PHYZQ		(1<<1)		// [1] : PHYZQENB
#define LOCK_PHYDLL			(1<<2)		// [2] : PHYDLOCK
#define UPDATE_FASTCH		(1<<3)		// [3] : Update Fast Channel Arbiter
#define UPDATE_SLOWCH		(1<<4)		// [4] : Update Slow Channel Arbiter
#define UPDATE_PLL			(1<<5)		// [5] : Update PLL settings

//------------------------------------------------------------------------------
#if (0)
static void		MemoryInitWithParameter( MEMORYCONFIG *pMemCfg )
{
	//--------------------------------------------------------------------------
	volatile U32 temp;

	NX_DEBUG_MSG( "Update Memory parameters...." );

	//----------------------------------------------------------------------
	// Update Fast Channel Arbiter
	if( pMemCfg->OPTION & UPDATE_FASTCH )
	{
		pMCUD->FASTCH[0]	= pMemCfg->FASTCH[0];
		pMCUD->FASTCH[1]	= pMemCfg->FASTCH[1];
		pMCUD->FASTCH[2]	= pMemCfg->FASTCH[2];
	}

	//----------------------------------------------------------------------
	// Update Slow Channel Arbiter
	if( pMemCfg->OPTION & UPDATE_SLOWCH )
	{
		pMCUD->SLOWCH[0]	= pMemCfg->SLOWCH[0];
		pMCUD->SLOWCH[1]	= pMemCfg->SLOWCH[1];
		pMCUD->SLOWCH[2]	= pMemCfg->SLOWCH[2];
	}

	//----------------------------------------------------------------------
	// Update MCUD Registers
	if( pMemCfg->OPTION & UPDATE_MCUD )
	{
		pMCUD->MEMCFG		= pMemCfg->CONFIG;
		pMCUD->MEMTIME0		= pMemCfg->TIME0;
		pMCUD->MEMTIME1		= pMemCfg->TIME1;

		pMCUD->PHYMODE		= pMemCfg->PHYMODE;

		//------------------------------------------------------------------
		pMCUD->PHYTERMCTRL	= pMemCfg->ZQENB;

		if( pMemCfg->OPTION & ENABLE_PHYZQ )
		{
			pMCUD->PHYZQCTRL	= 1;
			while( 1 )
			{
				temp = (pMCUD->PHYZQCTRL>>2) & 3;
				if( 1 == temp )		// ZQ END Check
				{
					break;
				}
				else if( 2 == temp )	// ZQ Error Check
				{
					// Error, do manual
					pMCUD->PHYZQCTRL	= 0;
					pMCUD->PHYZQCTRL	= pMemCfg->ZQSTART;
					pMCUD->PHYZQFORCE	= pMemCfg->ZQFORCE;
					break;
				}
			}
			pMCUD->PHYUPDATE	= pMemCfg->UPDATE;
		}

		//------------------------------------------------------------------
		pMCUD->PHYDELAYCTRL		= pMemCfg->DQSOFFSET;
		pMCUD->PHYDLLCTRL0		= pMemCfg->PHYDLL;

		//------------------------------------------------------------------
		if( pMemCfg->OPTION & LOCK_PHYDLL )
		{
			pMCUD->PHYDLLCTRL1	= pMemCfg->DLLLOCKSTART;
		}
		else
		{
			pMCUD->PHYDLLCTRL1	= 0;
			pMCUD->PHYDLLFORCE	= pMemCfg->DLLLOCKFORCE;
			pMCUD->PHYDLLCTRL1	= pMemCfg->DLLLOCKSTART;
		}

		//------------------------------------------------------------------
		pMCUD->MEMCTRL	= pMemCfg->ENABLE;
	}

	//----------------------------------------------------------------------
	// Update PLL Registers
	if( pMemCfg->OPTION & UPDATE_PLL )
	{
		pCLKPWR->CLKMODEREG0	= pMemCfg->CLKMODE[0];
		pCLKPWR->CLKMODEREG1	= pMemCfg->CLKMODE[1];
		pCLKPWR->PLLSETREG[0]	= pMemCfg->PLLSET [0];
		pCLKPWR->PLLSETREG[1]	= pMemCfg->PLLSET [1];

		pCLKPWR->PWRMODE |= REG_CLKPWR_PWRMODE_CHGPLL;
		while( pCLKPWR->PWRMODE & REG_CLKPWR_PWRMODE_CHGPLL );
	}

	//----------------------------------------------------------------------
	// Delay
	while( pMemCfg->DELAY-- )
	{
		temp = pCLKPWR->PWRMODE;
	}
	//--------------------------------------------------------------------------

	NX_DEBUG_MSG( "Completed\n" );
}
#endif

//------------------------------------------------------------------------------
CBOOL	ProcessBootHeader( U32 *pBootHeader, BOOTINFO *pBootInfo )
{
	NX_ASSERT( 0 == ((U32)pBootHeader & 3) );
	NX_ASSERT( CNULL != pBootInfo );

	#define	HEADER_ID		((((U32)'M')<< 0) | (((U32)'B')<< 8) | (((U32)'H')<<16) | (((U32)'F')<<24))

	if( pBootHeader[0] != HEADER_ID )
	{
		NX_DEBUG_MSG("Missing Boot header = 0x");
		NX_DEBUG_HEX(pBootHeader[0]);
		NX_DEBUG_MSG("\n");
		return CFALSE;
	}

	pBootInfo->iDownloadSize	= (int)pBootHeader[1];
	//pBootInfo->dwDownloadAddr	= (U32)pBootHeader[2];
	pBootInfo->dwJumpAddr		= (U32)pBootHeader[3];
	printf("Download Size = %d (%x)\n",pBootInfo->iDownloadSize,pBootInfo->iDownloadSize);
	NX_DEBUG_MSG("\n- Size : ");
	NX_DEBUG_HEX(pBootInfo->iDownloadSize);
	NX_DEBUG_MSG("(");
	NX_DEBUG_DEC(pBootInfo->iDownloadSize);
	NX_DEBUG_MSG(")\n");
	NX_DEBUG_MSG("- Addr : ");
	NX_DEBUG_HEX(pBootInfo->dwDownloadAddr);
	NX_DEBUG_MSG(", ");
	NX_DEBUG_HEX(pBootInfo->dwJumpAddr);
	NX_DEBUG_MSG("\n\n");

	//--------------------------------------------------------------------------
	//MemoryInitWithParameter( (MEMORYCONFIG *)&pBootHeader[4] );

	return CTRUE;
}
