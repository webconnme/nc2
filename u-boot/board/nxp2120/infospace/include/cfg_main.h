/*------------------------------------------------------------------------------
 *
 *	Copyright (C) 2005 Nexell Co., Ltd All Rights Reserved
 *	Nexell Co. Proprietary & Confidential
 *
 *	NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
 *  AND	WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
 *  BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
 *  FOR A PARTICULAR PURPOSE.
 *
 *	Module     : System memory config
 *	Description:
 *	Author     : Platform Team
 *	Export     :
 *	History    :
 *	   2009/05/13 first implementation
 ------------------------------------------------------------------------------*/
#ifndef __CFG_MAIN_H__
#define __CFG_MAIN_H__

#include <cfg_sys.h>

/*------------------------------------------------------------------------------
 * 	System Name
 */
#define	CFG_SYS_CPU_NAME						"nxp2120"
#define	CFG_SYS_BOARD_NAME						"nxp2120-dtk"

/*------------------------------------------------------------------------------
 * 	System PLL / BUS config
 */
/* cpu clock : fclk mclk bclk pclk */
#define	CFG_SYS_CLKPWR_UPDATE					CFALSE
#define	CFG_SYS_CLKPWR_SYNC_BUS					CFALSE

/* memory timing config : not stable */
#define CFG_SYS_MCUD_UPDATE						CFALSE
#define	CFG_SYS_MCUD_DLLRESET					CFALSE

/* memory bus arbiter config  */
#define CFG_SYS_UPDATE_FASTCH					CFALSE
#define CFG_SYS_UPDATE_SLOWCH					CFALSE

/*------------------------------------------------------------------------------
 * 	Debug Uart
 */
#define CFG_UART_DEBUG_CH						0
#define	CFG_UART_DEBUG_CLKSRC					CFG_SYS_CLKSRC_PLL1
#define	CFG_UART_DEBUG_CLKDIV					26 		// PLL1 147,500,000 Hz = 40 / PLL1 192,000,000 Hz = 26
#define	CFG_UART_DEBUG_CLKFREQ					(CFG_SYS_PLL1_FREQ/CFG_UART_DEBUG_CLKDIV)
#define CFG_UART_DEBUG_BAUDRATE					115200
#define CFG_UART_DEBUG_USE_UART					CTRUE

/*------------------------------------------------------------------------------
 * 	Timer List
 */
#define	CFG_TIMER_SYS_TICK_CH					0
#define	CFG_TIMER_SYS_TICK_CLKSRC				CFG_SYS_CLKSRC_PLL1
#define	CFG_TIMER_SYS_TICK_CLKDIV				64 		// (N divider:1~256)
#define	CFG_TIMER_SYS_TICK_CLKFREQ				(CFG_SYS_PLL1_FREQ/CFG_TIMER_SYS_TICK_CLKDIV)

#define	CFG_TIMER_SUB_TICK_CH					1		// For Timer driver
#define	CFG_TIMER_SUB_TICK_CLKSRC				CFG_SYS_CLKSRC_PLL1
#define	CFG_TIMER_SUB_TICK_CLKDIV				192		// (N divider:1~256)
#define	CFG_TIMER_SUB_TICK_CLKFREQ				(CFG_SYS_PLL1_FREQ/CFG_TIMER_SUB_TICK_CLKDIV)

#define	CFG_TIMER_WDT_TICK_CH					2		// For Timer driver
#define	CFG_TIMER_WDT_TICK_CLKSRC				CFG_SYS_CLKSRC_PLL1
#define	CFG_TIMER_WDT_TICK_CLKDIV				192		// (N divider:1~256)
#define	CFG_TIMER_WDT_TICK_CLKFREQ				(CFG_SYS_PLL1_FREQ/CFG_TIMER_SUB_TICK_CLKDIV)

/*------------------------------------------------------------------------------
 * 	Extern Ethernet
 */
#define CFG_EXT_PHY_BASEADDR_ETHER          	0x04000000	// DM9000: CS1
#define	CFG_EXT_IRQ_NUM_ETHER					(IRQ_GPIO_C_START + 12)

/*------------------------------------------------------------------------------
 * 	Nand
 */

/* nand interface */
#define CFG_NAND_ECC_MODE               		8							/* 4,8,16 */
#define CFG_NAND_ECC_LIMIT             			(CFG_NAND_ECC_MODE - 2)		/* ecc error correction limit */

#define CFG_NAND_MLC_ECC_MODE              		16
#define CFG_NAND_WRITEPROTECT					CFALSE

//#define	CFG_NAND_OPTIONS					NAND_NO_READRDY	/* For Hynix 64M (sector=16K, page=512) */

/*------------------------------------------------------------------------------
 * 	Display (DPC and MLC)
 */
/* oot status */
#define CFG_DISP_PRI_BOOT_LOGO                 	CTRUE
#define CFG_DISP_PRI_BOOT_ENB   	           	CTRUE

#define CFG_DISP_MAIN_SCREEN                 	MAIN_SCREEN_PRI	// FIX
#define CFG_DISP_HW_CURSOR_ENB              	CFALSE

/* MLC layer order */
#define CFG_DISP_LAYER_CURSOR                   0
#define CFG_DISP_LAYER_SCREEN                   1
#define CFG_DISP_LAYER_GRP3D                    1
#define CFG_DISP_LAYER_VIDEO                    3
#define CFG_DISP_LAYER_VIDEO_PRIORITY           2	// 0, 1, 2, 3

#define CFG_DISP_LAYER_CURSOR_NAME              "Cursor"
#define CFG_DISP_LAYER_SCREEN_NAME              "Window"
#define CFG_DISP_LAYER_GRP3D_NAME               "Grp3D"
#define CFG_DISP_LAYER_VIDEO_NAME               "Video"

/* MLC screen layer format */
#define CFG_DISP_SCREEN_RGB_FORMAT              MLC_RGBFMT_R5G6B5
#define CFG_DISP_SCREEN_PIXEL_BYTE	            2
#define CFG_DISP_SCREEN_COLOR_KEY	            0x090909
#define CFG_DISP_BACK_GROUND_COLOR	            0x0

/* Primary Display Module Sync */
#define CFG_DISP_PRI_RESOL_WIDTH          		800	// X Resolution
#define CFG_DISP_PRI_RESOL_HEIGHT				480	// Y Resolution

#define CFG_DISP_PRI_HSYNC_SYNC_WIDTH           1 
#define CFG_DISP_PRI_HSYNC_FRONT_PORCH          16
#define CFG_DISP_PRI_HSYNC_BACK_PORCH           46
#define CFG_DISP_PRI_HSYNC_ACTIVE_HIGH          CFALSE
#define CFG_DISP_PRI_VSYNC_SYNC_WIDTH           1
#define CFG_DISP_PRI_VSYNC_FRONT_PORCH          6
#define CFG_DISP_PRI_VSYNC_BACK_PORCH           6
#define CFG_DISP_PRI_VSYNC_ACTIVE_HIGH 	        CFALSE
#define CFG_DISP_PRI_VSYNC_START_OFFSET			1
#define CFG_DISP_PRI_VSYNC_END_OFFSET			1

#define CFG_DISP_PRI_EVSYNC_ACTIVE_HEIGHT       1	/* Not used */
#define CFG_DISP_PRI_EVSYNC_SYNC_WIDTH          1	/* Not used */
#define CFG_DISP_PRI_EVSYNC_FRONT_PORCH         1 	/* Not used */
#define CFG_DISP_PRI_EVSYNC_BACK_PORCH          1	/* Not used */
#define CFG_DISP_PRI_EVSYNC_START_OFFSET		1	/* Not used */
#define CFG_DISP_PRI_EVSYNC_END_OFFSET			1	/* Not used */

#define CFG_DISP_PRI_SYNC_DELAY_RGB_PVD			0
#define CFG_DISP_PRI_SYNC_DELAY_HS_CP1			7
#define CFG_DISP_PRI_SYNC_DELAY_VS_FRAM			7
#define CFG_DISP_PRI_SYNC_DELAY_DE_CP2			7

#define CFG_DISP_PRI_CLKGEN0_SOURCE             DPC_VCLK_SRC_PLL1
#define CFG_DISP_PRI_CLKGEN0_DIV                6
#define CFG_DISP_PRI_CLKGEN0_DELAY              0
#define CFG_DISP_PRI_CLKGEN1_SOURCE             DPC_VCLK_SRC_VCLK2
#define CFG_DISP_PRI_CLKGEN1_DIV                1
#define CFG_DISP_PRI_CLKGEN1_DELAY              0
#define CFG_DISP_PRI_PADCLKSEL                  DPC_PADCLKSEL_VCLK
#define CFG_DISP_PRI_OUT_CLK_INVERT             CFALSE

#define CFG_DISP_PRI_OUT_FORMAT                 DPC_FORMAT_RGB888
#define CFG_DISP_PRI_OUT_DUAL_VIEW              CFALSE
#define CFG_DISP_PRI_OUT_YCORDER                DPC_YCORDER_CbYCrY
#define CFG_DISP_PRI_OUT_RGB                    CTRUE
#define CFG_DISP_PRI_OUT_INTERLACE              CFALSE
#define CFG_DISP_PRI_OUT_POL_INVERT             CFALSE

#define CFG_DISP_PRI_OUT_SWAPRB                 CFALSE

#define CFG_DISP_PRI_MLC_INTERLACE              CFALSE
#define	CFG_DISP_PRI_MLC_LOCKSIZE				8

/* Primary out encoder */
#define CFG_CVBS_ENC_PRI_OUT_FORMAT		   		ENC_OUT_FMT_NONE
#define CFG_CVBS_ENC_PRI_OUT_TYPE               ENC_OUT_TYPE_RGB

/*------------------------------------------------------------------------------
 * 	PWM
 */
#define CFG_PWM_CLK_SOURCE						PWM_CLK_SRC_PLL1
#define CFG_PWM_CLK_FREQ						CFG_SYS_PLL1_FREQ
#define CFG_PWM_CLK_DIV							64
#define CFG_PWM_PERIOD							100		/* must be 100 (%) */

#define CFG_LCD_PRI_BLU_ON						CTRUE	/* PWM */
#define CFG_LCD_PRI_LCD_ON						CTRUE	/* LCD controller */

#define CFG_LCD_PRI_PWM_CH						0
#define CFG_LCD_PRI_PWM_FREQ					300
#define CFG_LCD_PRI_PWM_DUTYCYCLE				50		/* (%) */

/*------------------------------------------------------------------------------
 * 	Suspend mode and Reset
 */
#define CFG_PWR_SLEEP_MODE_ENB					CTRUE
#define CFG_PWR_SLEEP_SIGNATURE					0x50575200	/* PWR (ASCII) */

#endif /* __CFG_MAIN_H__ */
