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
#ifndef __CFG_GPIO_H__
#define __CFG_GPIO_H__

/*------------------------------------------------------------------------------
 *
 *	(GROUP_A)
 *
 *	0 bit  	  		 			     4 bit	   					     8 bit		  12 bit   				     16 bit
 *	| GPIO'A'OUTENB and GPIO'A'ALTFN | GPIO'A'OUT or GPIO'A'DETMODE0 | GPIO'A'PUENB| CLKPWR.PADSTRENGTHGPIO'A'|
 *
 -----------------------------------------------------------------------------*/
#define PAD_GPIOA0      (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)   	// 1: PVD0_0 	,2:_		,3:_		= B_0
#define PAD_GPIOA1      (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: PVD0_1 	,2:_		,3:_		= B_1
#define PAD_GPIOA2      (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: PVD0_2 	,2:_		,3:_		= B_2
#define PAD_GPIOA3      (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: PVD0_3 	,2:_		,3:_		= B_3
#define PAD_GPIOA4      (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: PVD0_4 	,2:_		,3:_		= B_4
#define PAD_GPIOA5      (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: PVD0_5 	,2:_		,3:_		= B_5
#define PAD_GPIOA6      (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: PVD0_6 	,2:_		,3:_		= B_6
#define PAD_GPIOA7      (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: PVD0_7 	,2:_		,3:_		= B_7
#define PAD_GPIOA8      (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: PVD0_8 	,2:_		,3:_		= G_0
#define PAD_GPIOA9      (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: PVD0_9 	,2:_		,3:_		= G_1
#define PAD_GPIOA10     (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: PVD0_10  	,2:_		,3:_		= G_2
#define PAD_GPIOA11     (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: PVD0_11  	,2:_		,3:_		= G_3
#define PAD_GPIOA12     (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: PVD0_12  	,2:_		,3:_		= G_4
#define PAD_GPIOA13     (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: PVD0_13  	,2:_		,3:_		= G_5
#define PAD_GPIOA14     (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: PVD0_14  	,2:_		,3:_		= G_6
#define PAD_GPIOA15     (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: PVD0_15  	,2:_		,3:_		= G_7
#define PAD_GPIOA16     (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: PVD0_16	,2:_		,3:_		= R_0
#define PAD_GPIOA17     (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: PVD0_17  	,2:_		,3:_		= R_1
#define PAD_GPIOA18     (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: PVD0_18  	,2:_		,3:_		= R_2
#define PAD_GPIOA19     (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: PVD0_29  	,2:_		,3:_		= R_3
#define PAD_GPIOA20     (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: PVD0_20  	,2:_		,3:_		= R_4
#define PAD_GPIOA21     (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: PVD0_21  	,2:_		,3:_		= R_5
#define PAD_GPIOA22     (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: PVD0_22  	,2:_		,3:_		= R_6
#define PAD_GPIOA23     (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: PVD0_23  	,2:_		,3:_		= R_7
#define PAD_GPIOA24     (PAD_MODE_ALT2 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_8) 	// 1: PVCLK  	,2: MPU_nCS	,3:_		= PVCLK
#define PAD_GPIOA25     (PAD_MODE_ALT2 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_8)     // 1: PDE 		,2: MPU_nWR	,3:_		= PDE
#define PAD_GPIOA26     (PAD_MODE_ALT2 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: PHSYNC	,2: MPU_nRD	,3:_		= HSYNC
#define PAD_GPIOA27     (PAD_MODE_ALT2 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: PVSYNC	,2: MPU_nRS	,3:_		= VSYNC
#define PAD_GPIOA28     (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: SPI0_FRM 	,2: SFnCS	,3:_		= EEPROM
#define PAD_GPIOA29     (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: SPI0_CLK 	,2: SFCLK	,3: VIDB[6]	= EEPROM
#define PAD_GPIOA30     (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: SPI0_RXD 	,2: SFDIN	,3: VIDB[7] = EEPROM
#define PAD_GPIOA31		(PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: SPI0_TXD 	,2: SFDOUT	,3: VCLKB2	= EEPROM

/*------------------------------------------------------------------------------
 *	(GROUP_B)
 *
 *	0 bit  	  		 			     4 bit	   					     8 bit		  12 bit   				     16 bit
 *	| GPIO'B'OUTENB and GPIO'B'ALTFN | GPIO'B'OUT or GPIO'B'DETMODE0 | GPIO'B'PUENB| CLKPWR.PADSTRENGTHGPIO'B'|
 *
 -----------------------------------------------------------------------------*/
#define PAD_GPIOB0		(PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)		// 1: UART0_TX,	2:_			,3:_		= UART0_TX
#define PAD_GPIOB1      (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)    	// 1: UART1_TX,	2:_			,3:_		= UART1_TX
#define PAD_GPIOB2      (PAD_MODE_IN   | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: PWMOUT0 ,	2:VCLKB		,3:_		= KEY_7
#define PAD_GPIOB3      (PAD_MODE_IN   | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: PWMOUT1 ,	2:SPDIF		,3:PPMI		= KEY_8
#define PAD_GPIOB4      (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: SCL0		,2:_		,3:_		= SCL0
#define PAD_GPIOB5      (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: SDA0		,2:_		,3:_		= SDA0
#define PAD_GPIOB6      (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)		// 1: SCL1		,2:_		,3:_		= SCL1
#define PAD_GPIOB7      (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: SDA1		,2:_		,3:_		= SDA1
#define PAD_GPIOB8      (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: I2S_DOUT	,2: AC97DOUT,3:_        = I2S_DOUT
#define PAD_GPIOB9      (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: I2S_BCLK 	,2: AC97BCLK,3:_        = I2S_BCLK
#define PAD_GPIOB10     (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: I2S_SYNC 	,2: AC97SYNC,3:_        = I2S_SYNC
#define PAD_GPIOB11     (PAD_MODE_IN   | PAD_OUT_LOWLEVEL    | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: I2S_DIN  	,2: AC97DIN	,3:_		= KEY_OK
#define PAD_GPIOB12     (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL    | PAD_PULLUP_OFF | PAD_STRENGTH_2)		// 1: I2S_MCLK 	,2: AC97nRST,3:_        = I2S_MCLK
#define PAD_GPIOB13     (PAD_MODE_IN   | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_8)		// 1: SDCLK		,2: TSICLK	,3: VIDB[0]	= KEY_L
#define PAD_GPIOB14     (PAD_MODE_IN   | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_8)		// 1: SDCMD  	,2: TSIDO	,3: VIDB[1]	= KEY_9
#define PAD_GPIOB15     (PAD_MODE_IN   | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_ON  | PAD_STRENGTH_8)		// 1: SDDAT0	,2: TSIERR	,3: VIDB[2]	= KEY_R
#define PAD_GPIOB16     (PAD_MODE_IN   | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_ON  | PAD_STRENGTH_8)		// 1: SDDAT1	,2: TSIDP	,3: VIDB[3]	= KEY_2
#define PAD_GPIOB17     (PAD_MODE_IN   | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_ON  | PAD_STRENGTH_8)		// 1: SDDAT2	,2: TSISYNC ,3: VIDB[4]	= NC
#define PAD_GPIOB18     (PAD_MODE_OUT  | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_ON  | PAD_STRENGTH_8)     // 1: SDDAT3	,2: -		,3: VIDB[5]	= RF_POWEREN
#define PAD_GPIOB19     (PAD_MODE_IN   | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: SD8  		,2:_		,3:_		= M1			???
#define PAD_GPIOB20     (PAD_MODE_IN   | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: SD9  		,2:_		,3:_		= FOR BOOT		???
#define PAD_GPIOB21     (PAD_MODE_IN   | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: SD10 		,2:_		,3:_		= FOR BOOT		???
#define PAD_GPIOB22     (PAD_MODE_IN   | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: SD11 		,2:_		,3:_		= KEY_6
#define PAD_GPIOB23     (PAD_MODE_OUT  | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: SD12 		,2:_		,3:_		= CAM_PWRDN
#define PAD_GPIOB24     (PAD_MODE_OUT  | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: SD13 		,2:_		,3:_		= CH1			???
#define PAD_GPIOB25     (PAD_MODE_OUT  | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: SD14 		,2:_		,3:_		= PWREN			???
#define PAD_GPIOB26     (PAD_MODE_IN   | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: SD15 		,2:_		,3:_		= CHARGE_DET
#define PAD_GPIOB27     (PAD_MODE_IN   | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: SA0  		,2:_		,3:_		= RST			???
#define PAD_GPIOB28     (PAD_MODE_IN   | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: SA1  		,2:_		,3:_		= KEY_5
#define PAD_GPIOB29     (PAD_MODE_OUT  | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)  	// 1: SA2  		,2:_		,3:_		= CH2			???
#define PAD_GPIOB30     (PAD_MODE_IN   | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: SA3  		,2:_		,3:_		= KEY_4
#define PAD_GPIOB31     (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)     // 1: VID[0]	,2:_		,3:_		= CAM_D0

/*------------------------------------------------------------------------------
 *	(GROUP_C)
 *
 *	0 bit  	  		 			     4 bit	   					     8 bit		  12 bit   				     16 bit
 *	| GPIO'C'OUTENB and GPIO'C'ALTFN | GPIO'C'OUT or GPIO'C'DETMODE0 | GPIO'C'PUENB| CLKPWR.PADSTRENGTHGPIO'C'|
 *
 -----------------------------------------------------------------------------*/
#define PAD_GPIOC0 		(PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)		// 1: VID1		,2:SA[13]	,3:_		= CAM_D1
#define PAD_GPIOC1      (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)		// 1: VID2		,2:SA[14]	,3:_		= CAM_D2
#define PAD_GPIOC2      (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)		// 1: VID3		,2:SA[15]	,3:RX4		= CAM_D3
#define PAD_GPIOC3      (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)		// 1: VID4		,2:SA[16]	,3:TX4		= CAM_D4
#define PAD_GPIOC4      (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)		// 1: VID5		,2:SA[17]	,3:_		= CAM_D5
#define PAD_GPIOC5      (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)		// 1: VID6		,2:SA[18]	,3:_		= CAM_D6
#define PAD_GPIOC6      (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)		// 1: VID7		,2:_		,3:_		= CAM_D7
#define PAD_GPIOC7      (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)		// 1: VCLK		,2:_		,3:_		= CAM_PCLK
#define PAD_GPIOC8      (PAD_MODE_ALT1 | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_ON  | PAD_STRENGTH_2)		// 1: UART1_RX	,2:_		,3:_		= UART1_RX
#define PAD_GPIOC9      (PAD_MODE_IN   | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)		// 1: SA4  		,2:_		,3:_		= KEY_3
#define PAD_GPIOC10     (PAD_MODE_INT  | PAD_INT_LOWLEVEL	 | PAD_PULLUP_ON  | PAD_STRENGTH_2)		// 1: SA5  		,2:_		,3:_		= PENIRQ
#define PAD_GPIOC11     (PAD_MODE_IN   | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)		// 1: SA6  		,2:_		,3:_		= KEY_1
#define PAD_GPIOC12     (PAD_MODE_IN   | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_ON  | PAD_STRENGTH_2)		// 1: SA7  		,2:_		,3:_		= KEY_DN
#define PAD_GPIOC13     (PAD_MODE_IN   | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_ON  | PAD_STRENGTH_8)		// 1: SA8  		,2:_		,3:_		= KEY_UP
#define PAD_GPIOC14     (PAD_MODE_IN   | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)		// 1: SA9  		,2:_		,3:_		= KEY_0
#define PAD_GPIOC15     (PAD_MODE_IN   | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)		// 1: SA10 		,2:_		,3:_		= KEY_ESC
#define PAD_GPIOC16     (PAD_MODE_IN   | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_ON  | PAD_STRENGTH_2)		// 1: SA11 		,2:_		,3:_		= POWER_KEY_DET
#define PAD_GPIOC17     (PAD_MODE_OUT  | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)		// 1: SA12 		,2:_		,3:_		= NAND_WP (ACTIVE LOW)
#define PAD_GPIOC18     (PAD_MODE_OUT  | PAD_OUT_LOWLEVEL	 | PAD_PULLUP_OFF | PAD_STRENGTH_2)		// 1: nSCS0		,2:nNCS2	,3:PWMOUT2	= LCD BL (BackLight)
#define PAD_GPIOC19     (PAD_MODE_OUT  | PAD_OUT_LOWLEVEL    | PAD_PULLUP_OFF | PAD_STRENGTH_2)		// 1: nSCS1		,2:nNCS3	,3:_		= LCD RST
#define PAD_GPIOC20     (PAD_MODE_ALT1 | PAD_OUT_HIGHLEVEL   | PAD_PULLUP_ON  | PAD_STRENGTH_2)		// 1: UART0_RX	,2:_		,3:_		= UART0_RX

/*------------------------------------------------------------------------------
 *	(GROUPALV)
 *	0  	   		   			4	   							8			12
 *	| MODE(IN/OUT/DETECT)	| ALIVE OUT or ALIVE DETMODE0 	|   PullUp	|
 *
 -----------------------------------------------------------------------------*/
#define	PAD_GPIOALV0	(PAD_MODE_IN  | PAD_OUT_LOWLEVEL	| PAD_PULLUP_ON )				// WAKE0
#define	PAD_GPIOALV1	(PAD_MODE_IN  | PAD_OUT_LOWLEVEL	| PAD_PULLUP_OFF)				// CH3
#define	PAD_GPIOALV2	(PAD_MODE_IN  | PAD_OUT_LOWLEVEL	| PAD_PULLUP_ON )				// M2	???
#define	PAD_GPIOALV3	(PAD_MODE_IN  | PAD_OUT_LOWLEVEL	| PAD_PULLUP_OFF)				// M3	???

/*------------------------------------------------------------------------------
 *	(BUS signal)		[BIT] :	0  	      8			12
 *								| BUS Pad | Strength |
 *
 -----------------------------------------------------------------------------*/
#define	PAD_BUS_STATIC_CNTL		(NX_CLKPWR_BUSPAD_STATIC_CNTL 	| PAD_STRENGTH_8)
#define	PAD_BUS_STATIC_ADDR		(NX_CLKPWR_BUSPAD_STATIC_ADDR 	| PAD_STRENGTH_8)
#define	PAD_BUS_STATIC_DATA		(NX_CLKPWR_BUSPAD_STATIC_DATA 	| PAD_STRENGTH_8)
#define	PAD_BUS_VSYNC			(NX_CLKPWR_BUSPAD_VSYNC 		| PAD_STRENGTH_8)
#define	PAD_BUS_HSYNC			(NX_CLKPWR_BUSPAD_HSYNC 		| PAD_STRENGTH_8)
#define	PAD_BUS_DE				(NX_CLKPWR_BUSPAD_DE 			| PAD_STRENGTH_8)

/*------------------------------------------------------------------------------
 *	GPIO I2C
 */
#define	CFG_PIO_I2C0_SCL						(PAD_GPIO_B +  4)
#define	CFG_PIO_I2C0_SDA						(PAD_GPIO_B +  5)
#define	CFG_PIO_I2C1_SCL						(PAD_GPIO_B +  6)
#define	CFG_PIO_I2C1_SDA						(PAD_GPIO_B +  7)

/*------------------------------------------------------------------------------
 *	Touch
 */
#define CFG_PIO_TOUCH_PENDOWN_DETECT			(PAD_GPIO_C + 10)

/*------------------------------------------------------------------------------
 *	LCD
 */
#define CFG_PIO_LCD_PCI_ENB						(PAD_GPIO_C + 19)

#endif	/* __CFG_GPIO_H__ */
