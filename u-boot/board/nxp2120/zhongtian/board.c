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
#include <config.h>
#include <common.h>

/* nexell soc headers */
#include <platform.h>
#include <platfunc.h>

DECLARE_GLOBAL_DATA_PTR;

/* debug macro */
#define DBGOUT(msg...)		{ printf(msg); }

extern int  load_boot_logo(void);
extern void nx_boot_logo(U32 framebase, int x_resol, int y_resol, U32 pixelbyte);

/*------------------------------------------------------------------------------
 * intialize nexell soc and board status.
 */
static void init_gpio_pad(void);
static void init_alive_pad(void);
static void init_bus_pad(void);
static void init_display(int dev);

int board_init(void)
{
	init_gpio_pad();
	init_alive_pad();
	init_bus_pad();

	DBGOUT("%s : done board initialize ...\n", CFG_SYS_BOARD_NAME);
	return 0;
}

int board_late_init(void)
{
	printf("Name : %s \n\n", CFG_SYS_BOARD_NAME);
	load_boot_logo();
	init_display(CFG_DISP_MAIN_SCREEN);
	return 0;
}

/*------------------------------------------------------------------------------
 * intialize nexell soc gpio pad func.
 */
static void init_gpio_pad(void)
{
	int  io_grp, io_bit;
	U32  io_mod, outval, detect, pullup, strength;

	const U32 io_pad[NUMBER_OF_GPIO_MODULE][32] = {
		/* GPIO group A */
		{
		PAD_GPIOA0,  PAD_GPIOA1,  PAD_GPIOA2,  PAD_GPIOA3,  PAD_GPIOA4,  PAD_GPIOA5,  PAD_GPIOA6,  PAD_GPIOA7,  PAD_GPIOA8,  PAD_GPIOA9,
		PAD_GPIOA10, PAD_GPIOA11, PAD_GPIOA12, PAD_GPIOA13, PAD_GPIOA14, PAD_GPIOA15, PAD_GPIOA16, PAD_GPIOA17, PAD_GPIOA18, PAD_GPIOA19,
		PAD_GPIOA20, PAD_GPIOA21, PAD_GPIOA22, PAD_GPIOA23, PAD_GPIOA24, PAD_GPIOA25, PAD_GPIOA26, PAD_GPIOA27, PAD_GPIOA28, PAD_GPIOA29,
		PAD_GPIOA30, PAD_GPIOA31
		},
		/* GPIO group B */
		{
		PAD_GPIOB0,  PAD_GPIOB1 , PAD_GPIOB2 , PAD_GPIOB3,  PAD_GPIOB4,  PAD_GPIOB5,  PAD_GPIOB6,  PAD_GPIOB7,  PAD_GPIOB8,  PAD_GPIOB9,
		PAD_GPIOB10, PAD_GPIOB11, PAD_GPIOB12, PAD_GPIOB13, PAD_GPIOB14, PAD_GPIOB15, PAD_GPIOB16, PAD_GPIOB17, PAD_GPIOB18, PAD_GPIOB19,
		PAD_GPIOB20, PAD_GPIOB21, PAD_GPIOB22, PAD_GPIOB23, PAD_GPIOB24, PAD_GPIOB25, PAD_GPIOB26, PAD_GPIOB27, PAD_GPIOB28, PAD_GPIOB29,
		PAD_GPIOB30, PAD_GPIOB31
		},
		/* GPIO group C */
		{
		PAD_GPIOC0,  PAD_GPIOC1,  PAD_GPIOC2,  PAD_GPIOC3,  PAD_GPIOC4,  PAD_GPIOC5,  PAD_GPIOC6,  PAD_GPIOC7,  PAD_GPIOC8,  PAD_GPIOC9,
		PAD_GPIOC10, PAD_GPIOC11, PAD_GPIOC12, PAD_GPIOC13, PAD_GPIOC14, PAD_GPIOC15, PAD_GPIOC16, PAD_GPIOC17, PAD_GPIOC18, PAD_GPIOC19,
		PAD_GPIOC20, 		   0, 		    0,  	     0,           0,           0,           0,           0,           0,           0,
		          0,           0
		}
	};

	/* GPIO pad function */
	for (io_grp = 0; NUMBER_OF_GPIO_MODULE > io_grp; io_grp++) {

		NX_GPIO_ClearInterruptPendingAll(io_grp);

		for (io_bit = 0; 32 > io_bit; io_bit++) {

			if (! io_pad[io_grp][io_bit])
				continue;

			io_mod = PAD_GET_PADMODE(io_pad[io_grp][io_bit]);

			switch (io_mod) {
			case PAD_MODE_IN:
				NX_GPIO_SetPadFunction (io_grp, io_bit, NX_GPIO_PADFUNC_GPIO);
				NX_GPIO_SetOutputEnable(io_grp, io_bit, CFALSE);
				break;
			case PAD_MODE_OUT:
				NX_GPIO_SetPadFunction (io_grp, io_bit, NX_GPIO_PADFUNC_GPIO);
				NX_GPIO_SetOutputEnable(io_grp, io_bit, CTRUE);
				break;
			case PAD_MODE_ALT1:
				NX_GPIO_SetPadFunction (io_grp, io_bit, NX_GPIO_PADFUNC_1);
				NX_GPIO_SetOutputEnable(io_grp, io_bit, CFALSE);
				break;
			case PAD_MODE_ALT2:
				NX_GPIO_SetPadFunction (io_grp, io_bit, NX_GPIO_PADFUNC_2);
				NX_GPIO_SetOutputEnable(io_grp, io_bit, CFALSE);
				break;
			case PAD_MODE_ALT3:
				NX_GPIO_SetPadFunction (io_grp, io_bit, NX_GPIO_PADFUNC_3);
				NX_GPIO_SetOutputEnable(io_grp, io_bit, CFALSE);
				break;
			case PAD_MODE_INT:
				detect = PAD_GET_DECTMODE(io_pad[io_grp][io_bit]);
				NX_GPIO_SetPadFunction(io_grp, io_bit, NX_GPIO_PADFUNC_GPIO);
				NX_GPIO_SetOutputEnable(io_grp, io_bit, CFALSE);
				NX_GPIO_SetInterruptMode(io_grp, io_bit, detect);
				break;
			case PAD_MODE_SKIP:
			case PAD_NOTEXIST:
			default:
				continue;
			}
			outval   = PAD_GET_OUTLEVEL(io_pad[io_grp][io_bit]);
			pullup   = PAD_GET_PULLUP(io_pad[io_grp][io_bit]);
			strength = PAD_GET_STRENGTH(io_pad[io_grp][io_bit]);

			NX_GPIO_SetOutputValue (io_grp, io_bit, (outval ? CTRUE : CFALSE));
			NX_GPIO_SetPullUpEnable(io_grp, io_bit, (pullup ? CTRUE : CFALSE));
			NX_CLKPWR_SetGPIOPadStrength(io_grp, io_bit, strength);
		}
	}
}

static void init_alive_pad(void)
{
	int  io_bit, det_mod;
	U32  io_mod, outval, detect, pullup, io_num;

	const U32 alv_pad[] = {
		PAD_GPIOALV0, PAD_GPIOALV1,  PAD_GPIOALV2, PAD_GPIOALV3
			};

	io_num = sizeof(alv_pad)/sizeof(alv_pad[0]);

	/* Alive pad function */
	for (io_bit = 0; io_num > io_bit; io_bit++) {

		NX_ALIVE_ClearInterruptPending(io_bit);
		io_mod = PAD_GET_PADMODE(alv_pad[io_bit]);

		switch (io_mod) {
		case PAD_MODE_IN   :
		case PAD_MODE_DECT :
			NX_ALIVE_SetOutputEnable(io_bit, CFALSE);
			break;
		case PAD_MODE_OUT  :
			NX_ALIVE_SetOutputEnable(io_bit, CTRUE);
			break;
		default :
			DBGOUT("\n Unknown GPIO ALIVE Mode(0x%x)\n", io_mod);
			continue;
		}
		outval = PAD_GET_PADMODE(alv_pad[io_bit]);
		pullup = PAD_GET_PULLUP(alv_pad[io_bit]);
		detect = PAD_GET_DECTMODE(alv_pad[io_bit]);

		NX_ALIVE_SetOutputValue (io_bit, (outval ? CTRUE : CFALSE));
		NX_ALIVE_SetPullUpEnable(io_bit, (pullup ? CTRUE : CFALSE));

		// set detect mode
		for (det_mod = 0; 6 > det_mod; det_mod++) {
			if (io_mod == PAD_MODE_DECT)
				NX_ALIVE_SetDetectMode(det_mod, io_bit, (det_mod == detect ? CTRUE : CFALSE));
			else
				NX_ALIVE_SetDetectMode(det_mod, io_bit, CFALSE);
		}
		NX_ALIVE_SetDetectEnable(io_bit, (io_mod == PAD_MODE_DECT ? CTRUE : CFALSE));
	}
}

static void init_bus_pad(void)
{
	int  io_bit;
	U32  strength, buspad, busnum;

	const U32 bus_pad[] = {
		PAD_BUS_STATIC_CNTL, PAD_BUS_STATIC_ADDR,  PAD_BUS_STATIC_DATA, PAD_BUS_VSYNC, PAD_BUS_HSYNC, PAD_BUS_DE
			};

	busnum = sizeof(bus_pad)/sizeof(bus_pad[0]);

	/* BUS pad function */
	for (io_bit = 0; busnum > io_bit; io_bit++) {
		buspad   = PAD_GET_BUS(bus_pad[io_bit]);
		strength = PAD_GET_STRENGTH(bus_pad[io_bit]);
		NX_CLKPWR_SetBusPadStrength(buspad, strength);
	}
}

/*------------------------------------------------------------------------------
 * intialize nexell soc display controller register.
 */
struct pwm_pad {
	unsigned int group;
	unsigned int bit;
	unsigned int func;
};

static struct pwm_pad pwm_ch[] = {
	{ PAD_GET_GRP(PAD_GPIO_B),  2, NX_GPIO_PADFUNC_1 },
	{ PAD_GET_GRP(PAD_GPIO_B),  3, NX_GPIO_PADFUNC_1 },
	{ PAD_GET_GRP(PAD_GPIO_C), 18, NX_GPIO_PADFUNC_3 }
};

#define PWM_PERIOD_MIN		2
#define PWM_PERIOD_MAX		1000
#define PWM_PRESCL_MIN		1
#define PWM_PRESCL_MAX		128

#define PWM_CLOCK_SOURCE	(CFG_PWM_CLK_SOURCE)

#define TMP_DIVISOR			((CFG_PWM_CLK_FREQ + 80000000 - 1)/80000000)	/* peri max clock is 80Mhz */
#if (TMP_DIVISOR & 0x1)														/* support only even value */
#define PWM_CLOCK_DIVISOR	(TMP_DIVISOR + 1)
#else
#define PWM_CLOCK_DIVISOR	(TMP_DIVISOR)
#endif

#define PWM_FREQUENCY_MAX	(CFG_PWM_CLK_FREQ/PWM_CLOCK_DIVISOR)
#define PWM_FREQUENCY_MIN	(CFG_PWM_CLK_FREQ/PWM_PRESCL_MAX/PWM_PERIOD_MAX)

/*------------------------------------------------------------------------------
 * intialize nexell soc display controller register.
 */
static void mpu_lcd_init(void);

static void init_display(int dev)
{
    U32  XResol	   = CFG_DISP_PRI_RESOL_WIDTH;
    U32  YResol	   = CFG_DISP_PRI_RESOL_HEIGHT;
	U32	 LockSize  = CFG_DISP_PRI_MLC_LOCKSIZE;

	U32  PFrameBase= (CFG_MEM_PHY_BLOCK_BASE ? CFG_MEM_PHY_BLOCK_BASE : CFG_MEM_PHY_LINEAR_BASE);
	U32  VFrameBase= (U32)IO_ADDRESS(PFrameBase);

	if (CFALSE == CFG_DISP_PRI_BOOT_ENB)
		return;

	//--------------------------------------------------------------------------
	// DPC Clock Disalbe
	//--------------------------------------------------------------------------
	NX_DPC_SetClockPClkMode(dev, NX_PCLKMODE_DYNAMIC);
	NX_DPC_SetDPCEnable(dev, CFALSE);
	NX_DPC_SetClockDivisorEnable(dev, CFALSE);

	//--------------------------------------------------------------------------
	// LCD On
	//--------------------------------------------------------------------------
#if	(CFG_LCD_PRI_LCD_ON)
	NX_GPIO_SetOutputValue(PAD_GET_GRP(CFG_PIO_LCD_PCI_ENB), PAD_GET_BIT(CFG_PIO_LCD_PCI_ENB), CTRUE);
#endif

	//--------------------------------------------------------------------------
	// Set MLC
	//--------------------------------------------------------------------------
	// RGB TOP Layer
	//
	NX_MLC_SetClockPClkMode(dev, NX_PCLKMODE_DYNAMIC);
	NX_MLC_SetClockBClkMode(dev, NX_BCLKMODE_DYNAMIC);
	NX_MLC_SetLayerPriority(dev, CFG_DISP_LAYER_VIDEO_PRIORITY);
	NX_MLC_SetBackground(dev, CFG_DISP_BACK_GROUND_COLOR);
	NX_MLC_SetFieldEnable(dev, CFG_DISP_PRI_MLC_INTERLACE);
	NX_MLC_SetScreenSize(dev, XResol, YResol);
	NX_MLC_SetRGBLayerGamaTablePowerMode(dev, CFALSE, CFALSE, CFALSE);
	NX_MLC_SetRGBLayerGamaTableSleepMode(dev, CTRUE, CTRUE, CTRUE);
	NX_MLC_SetRGBLayerGammaEnable(dev, CFALSE);
	NX_MLC_SetDitherEnableWhenUsingGamma(dev, CFALSE);
	NX_MLC_SetGammaPriority(dev, CFALSE);
    NX_MLC_SetTopPowerMode(dev, CTRUE);
    NX_MLC_SetTopSleepMode(dev, CFALSE);
	NX_MLC_SetMLCEnable(dev, CTRUE);
	NX_MLC_SetTopDirtyFlag(dev);

	// RGB Layer
	//
	if (CFALSE == CFG_DISP_PRI_BOOT_LOGO)
		return;

	NX_MLC_SetLockSize(dev, CFG_DISP_LAYER_SCREEN, LockSize);
	NX_MLC_SetAlphaBlending(dev, CFG_DISP_LAYER_SCREEN, CFALSE, 15);
	NX_MLC_SetTransparency(dev, CFG_DISP_LAYER_SCREEN, CFALSE, 0);
	NX_MLC_SetColorInversion(dev, CFG_DISP_LAYER_SCREEN, CFALSE, 0);
	NX_MLC_SetRGBLayerInvalidPosition(dev, CFG_DISP_LAYER_SCREEN, 0, 0, 0, 0, 0, CFALSE);
	NX_MLC_SetRGBLayerInvalidPosition(dev, CFG_DISP_LAYER_SCREEN, 1, 0, 0, 0, 0, CFALSE);
	NX_MLC_SetFormatRGB(dev, CFG_DISP_LAYER_SCREEN, CFG_DISP_SCREEN_RGB_FORMAT);
	NX_MLC_SetPosition(dev, CFG_DISP_LAYER_SCREEN, 0, 0, XResol-1, YResol-1);

	// Draw logo
	//
	nx_boot_logo(VFrameBase, XResol, YResol, CFG_DISP_SCREEN_PIXEL_BYTE);

	// RGB layer Eanble
	//
	NX_MLC_SetRGBLayerStride(dev, CFG_DISP_LAYER_SCREEN, CFG_DISP_SCREEN_PIXEL_BYTE, XResol*CFG_DISP_SCREEN_PIXEL_BYTE);
	NX_MLC_SetRGBLayerAddress(dev, CFG_DISP_LAYER_SCREEN, PFrameBase);

	NX_MLC_SetLayerEnable(dev, CFG_DISP_LAYER_SCREEN, CTRUE);
	NX_MLC_SetDirtyFlag(dev, CFG_DISP_LAYER_SCREEN);

	//--------------------------------------------------------------------------
	// Set DPC
	//--------------------------------------------------------------------------
	mpu_lcd_init();

	//--------------------------------------------------------------------------
	// LCD Backlight On
	//--------------------------------------------------------------------------
	if (CFG_LCD_PRI_BLU_ON == CTRUE) {
		int scl = 0, peri = 0, duty = 0;

		NX_PWM_SetClockPClkMode(NX_PCLKMODE_ALWAYS);
		NX_PWM_SetClockDivisorEnable(CFALSE);
		NX_PWM_SetClockSource(0, PWM_CLOCK_SOURCE);
		NX_PWM_SetClockDivisor(0, PWM_CLOCK_DIVISOR);

		if (CFG_LCD_PRI_PWM_FREQ > PWM_FREQUENCY_MAX/PWM_PERIOD_MAX) {
			scl  = PWM_PRESCL_MIN;
			peri = (PWM_FREQUENCY_MAX/scl)/CFG_LCD_PRI_PWM_FREQ;
		} else {
			peri = PWM_PERIOD_MAX;
			scl  = (PWM_FREQUENCY_MAX/peri)/CFG_LCD_PRI_PWM_FREQ;
		}
		duty = (peri * CFG_LCD_PRI_PWM_DUTYCYCLE) / 100;

		NX_PWM_SetPreScale(CFG_LCD_PRI_PWM_CH, scl);
		NX_PWM_SetPeriod(CFG_LCD_PRI_PWM_CH, peri);
		NX_PWM_SetDutyCycle(CFG_LCD_PRI_PWM_CH, duty);
		NX_PWM_SetClockDivisorEnable(CTRUE);

		/* Enable pwm out pad*/
		NX_GPIO_SetPadFunction(pwm_ch[CFG_LCD_PRI_PWM_CH].group,
			pwm_ch[CFG_LCD_PRI_PWM_CH].bit, (NX_GPIO_PADFUNC)pwm_ch[CFG_LCD_PRI_PWM_CH].func);
	}
}

/*----------------------------------------------------------------------------*/
#define	LCD_WIDTH			CFG_DISP_PRI_RESOL_WIDTH
#define	LCD_HFP 			CFG_DISP_PRI_HSYNC_FRONT_PORCH
#define	LCD_HBP				CFG_DISP_PRI_HSYNC_BACK_PORCH
#define	LCD_HSW				CFG_DISP_PRI_HSYNC_SYNC_WIDTH

#define	LCD_HEIGHT			CFG_DISP_PRI_RESOL_HEIGHT
#define	LCD_VFP				CFG_DISP_PRI_VSYNC_FRONT_PORCH
#define	LCD_VBP				CFG_DISP_PRI_VSYNC_BACK_PORCH
#define	LCD_VSW				CFG_DISP_PRI_VSYNC_SYNC_WIDTH

#define DPC_CLKSRCSEL0		1	// PLL1
#define DPC_CLKDIV0			CFG_DISP_PRI_CLKGEN0_DIV
#define DPC_OUTCLKINV0		1

#define DPC_CLKSRCSEL1		7	// Reserved
#define DPC_CLKDIV1			1
#define DPC_OUTCLKINV1		1

#define	MPU_DATABIT_16		1

/*----------------------------------------------------------------------------*/
#define SYNCGEN_BASEADDRESS 	IO_ADDRESS(0xC0003000)	// <== dpc base address

#define	SYNCGENTOTHLENG			SYNCGEN_BASEADDRESS+(62 * 2)
#define	SYNCGENHSBEGIN			SYNCGEN_BASEADDRESS+(63 * 2)
#define	SYNCGENHABEGIN			SYNCGEN_BASEADDRESS+(64 * 2)
#define	SYNCGENHAEND			SYNCGEN_BASEADDRESS+(65 * 2)
#define	SYNCGENTOTVLENG			SYNCGEN_BASEADDRESS+(66 * 2)
#define	SYNCGENVSBEGIN			SYNCGEN_BASEADDRESS+(67 * 2)
#define	SYNCGENVABEGIN			SYNCGEN_BASEADDRESS+(68 * 2)
#define	SYNCGENVAEND			SYNCGEN_BASEADDRESS+(69 * 2)
#define	SYNCGENCTRL0			SYNCGEN_BASEADDRESS+(70 * 2)
#define	SYNCGENCTRL1			SYNCGEN_BASEADDRESS+(71 * 2)
#define	SYNCGENEVENTOTVLENG    	SYNCGEN_BASEADDRESS+(72 * 2)
#define	SYNCGENEVENVSBEGIN	    SYNCGEN_BASEADDRESS+(73 * 2)
#define	SYNCGENEVENVABEGIN	    SYNCGEN_BASEADDRESS+(74 * 2)
#define	SYNCGENEVENVAEND		SYNCGEN_BASEADDRESS+(75 * 2)
#define	SYNCGENCTRL2			SYNCGEN_BASEADDRESS+(76 * 2)
#define	SYNCGENVSETPIXEL    	SYNCGEN_BASEADDRESS+(77 * 2)
#define	SYNCGENVCLRPIXEL    	SYNCGEN_BASEADDRESS+(78 * 2)
#define	SYNCGENEVENVSETPIXEL	SYNCGEN_BASEADDRESS+(79 * 2)
#define	SYNCGENEVENVCLRPIXEL	SYNCGEN_BASEADDRESS+(80 * 2)
#define	SYNCGENDELAY0			SYNCGEN_BASEADDRESS+(81 * 2)
#define	SYNCGENHSCALEL			SYNCGEN_BASEADDRESS+(82 * 2)
#define	SYNCGENHSCALEH			SYNCGEN_BASEADDRESS+(83 * 2)
#define	SYNCGENSOURCEWIDTH		SYNCGEN_BASEADDRESS+(84 * 2)
#define	SYNCGENDELAY1			SYNCGEN_BASEADDRESS+(94 * 2)
#define	SYNCGENMPUTTIME0		SYNCGEN_BASEADDRESS+(95 * 2)	// [15:8]: setup [7:0]: hold
#define	SYNCGENMPUTTIME1		SYNCGEN_BASEADDRESS+(96 * 2)	// [7:0]: access
#define	SYNCGENMPUTWDATA		SYNCGEN_BASEADDRESS+(97 * 2)	// [7:0]: Write[15:0], Index[15:0]
#define	SYNCGENMPUTLOADINDEX	SYNCGEN_BASEADDRESS+(98 * 2)	// [7:0]: index[23:0]
#define	SYNCGENMPUTSTATUS		SYNCGEN_BASEADDRESS+(99 * 2)	// [7:0]: access
#define	SYNCGENMPUTLOADATA		SYNCGEN_BASEADDRESS+(100* 2)	// [15:0]: Write[23:16], Read[23:16]
#define	SYNCGENMPUTRDATAH		SYNCGEN_BASEADDRESS+(101* 2)	// [7:0]: Read[15:0]

#define	CLKGENBASE				(512-64)
#define	CLKGENENB				SYNCGEN_BASEADDRESS+CLKGENBASE
#define	CLKGEN0DVOL				SYNCGEN_BASEADDRESS+CLKGENBASE+4
#define	CLKGEN0DVOH				SYNCGEN_BASEADDRESS+CLKGENBASE+8
#define	CLKGEN1DVOL				SYNCGEN_BASEADDRESS+CLKGENBASE+12
#define	CLKGEN1DVOH				SYNCGEN_BASEADDRESS+CLKGENBASE+16

// format
#define RGB555		0x0
#define RGB565		0x1	// RGB565B
#define RGB666		0x2	// RGB666B
#define RGB888		0x3
#define MRGB555A	0x4
#define MRGB555B	0x5
#define MRGB565		0x6
#define MRGB666		0x7
#define MRGB888A	0x8
#define MRGB888B	0x9
#define CCIR656 	0xA
#define CCIR601_8 	0xB
#define CCIR601_16A	0xC
#define CCIR601_16B	0xD
#define SRGB888		0xE	// serial (ODD : R8->G8->B8), (EVEN : R8->G8->B8)
#define SRGBD8888	0xF	// serial (ODD,EVEN : R8->G8->B8->dummy)

#define FORMAT_MASK	0xF

#define RGBX6XA		(1<<4)		// RGB666A, RGB565A
#define RGBX6XB		(0<<4)		// RGB666B, RGB565B


#define RGB565A		(RGB565 | RGBX6XA)
#define RGB666A		(RGB565 | RGBX6XA)

#define  RGBAB_MASK	(1<<4)

// lcd size & time
#define tHFP		LCD_HFP	// 1
#define tHBP		LCD_HBP // 100
#define tHSW		LCD_HSW	// 1
#define tHACT   	LCD_WIDTH

#define tVFP		LCD_VFP	// 1
#define tVBP		LCD_VBP	// 40
#define tVSW		LCD_VSW	// 1
#define tVACT		LCD_HEIGHT

#define	DELAY_CMD	do { udelay(3*1000); } while (0);
#define	DELAY_DAT	do { udelay(3*1000); } while (0);

static void MPU_COMMAND(unsigned int cmd)
{
	unsigned short DL  = 0;
	unsigned short DH  = 0;

	// command write
	//nCS-----             -------------
	//       |_____________|
	//nRS
	//   _______________________________
	//nWR------          ---------------
	//        |__________|
#if (MPU_DATABIT_16)
	unsigned int RGB = 0;
	RGB = ((cmd&0xF800)<<8)| ((cmd&0x07E0)<<5) | ((cmd&0x001F)<<3);	// R5 | G6 | B5
	DL  = ((RGB>> 0) & 0xFFFF);
	DH  = ((RGB>>16) & 0xFFFF);
	*(volatile unsigned short*)(SYNCGENMPUTWDATA)		= DL;	// W L
	*(volatile unsigned short*)(SYNCGENMPUTLOADINDEX)	= DH;	// W H
#else
	DL = ((cmd>> 0)&0xFF);
	DL = ((DL & 0xE0) << 3) | ((DL & 0x1F) << 3);

	DH = ((cmd>>16)&0xFF);
	DH = ((DH & 0xE0) << 3) | ((DH & 0x1F) << 3);

	*(volatile unsigned short*)(SYNCGENMPUTWDATA)		= DH;	// W L
	*(volatile unsigned short*)(SYNCGENMPUTLOADINDEX)	= 0;	// W H

	*(volatile unsigned short*)(SYNCGENMPUTWDATA)		= DL;	// W L
	*(volatile unsigned short*)(SYNCGENMPUTLOADINDEX)	= 0;	// W H
#endif
	DELAY_CMD;
}

static void MPU_WRITE_D(unsigned int data)
{
	unsigned short DL  = 0;
	unsigned short DH  = 0;

	// data write
	//nCS-----             -------------
	//       |_____________|
	//nRS       ----------
	//   _____|          |______________
	//nWR------          ---------------
	//        |__________|
	//udelay(1);
#if (MPU_DATABIT_16)
	unsigned int RGB = 0;
	RGB = ((data&0xF800)<<8)| ((data&0x07E0)<<5) | ((data&0x001F)<<3);	// R5 | G6 | B5
	DL  = ((RGB>> 0) & 0xFFFF);
	DH  = ((RGB>>16) & 0xFFFF);

	*(volatile unsigned short*)(SYNCGENMPUTWDATA)	= DL;	// W L
	*(volatile unsigned short*)(SYNCGENMPUTLOADATA)	= DH;	// W H
#else
	DL = ((data>> 0)&0xFF);
	DL = ((DL & 0xE0) << 3) | ((DL & 0x1F) << 3);

	DH = ((data>>16)&0xFF);
	DH = ((DH & 0xE0) << 3) | ((DH & 0x1F) << 3);

	*(volatile unsigned short*)(SYNCGENMPUTWDATA)		= DH;	// W L
	*(volatile unsigned short*)(SYNCGENMPUTLOADINDEX)	= 0;	// W H

	*(volatile unsigned short*)(SYNCGENMPUTWDATA)		= DL;	// W L
	*(volatile unsigned short*)(SYNCGENMPUTLOADINDEX)	= 0;	// W H
#endif

	DELAY_DAT;
}

static void MPU_READ_CD(unsigned int cmd, unsigned int *pdat, int len)
{
	unsigned short DL  = 0;
	unsigned short DH  = 0;
	int i = 0;

	// command write
	//nCS-----             -------------
	//       |_____________|
	//nRS
	//   _______________________________
	//nWR------          ---------------
	//        |__________|
#if (MPU_DATABIT_16)
	unsigned int RGB = 0;

	RGB = ((cmd&0xF800)<<8)| ((cmd&0x07E0)<<5) | ((cmd&0x001F)<<3);	// R5 | G6 | B5
	DL  = ((RGB>> 0) & 0xFFFF);
	DH  = ((RGB>>16) & 0xFFFF);

	*(volatile unsigned short*)(SYNCGENMPUTWDATA)		= DL;	// W L
	*(volatile unsigned short*)(SYNCGENMPUTLOADINDEX)	= DH;	// W H

//	DELAY_CMD;

	// data read
	//nCS-----             -------------
	//       |_____________|
	//nRS       ----------
	//   _____|          |______________
	//nWR------          ---------------
	//        |__________|

	while (len > i) {
		DH  = *(volatile unsigned short*)(SYNCGENMPUTLOADATA);	// R H
		DL  = *(volatile unsigned short*)(SYNCGENMPUTRDATAH);	// R L
		pdat[i++] = ((DH&0x00F8)<<4) | ((DL&0xFC00)>>5) | ((DL&0x00F8)>>3);
	}
	DELAY_DAT;
#else
	// command write
	DL = ((cmd>> 0)&0xFF);
	DL = ((DL & 0xE0) << 3) | ((DL & 0x1F) << 3);

	DH = ((cmd>>16)&0xFF);
	DH = ((DH & 0xE0) << 3) | ((DH & 0x1F) << 3);

	*(volatile unsigned short*)(SYNCGENMPUTWDATA)		= DH;
	*(volatile unsigned short*)(SYNCGENMPUTLOADINDEX)	= 0;

	*(volatile unsigned short*)(SYNCGENMPUTWDATA)		= DL;
	*(volatile unsigned short*)(SYNCGENMPUTLOADINDEX)	= 0;

	DELAY_CMD;

	// data read
	while (len > i) {
		unsigned short DD[2] = { 0, };
		unsigned short TD[2] = { 0, };
		int n = 2;
		for (; n>0; n--) {
			TD[n] = *(volatile unsigned short*)(SYNCGENMPUTLOADATA);	// R H
			DD[n] = *(volatile unsigned short*)(SYNCGENMPUTRDATAH);		// R L
		}
		DH = DD[0];
		DL = DD[1];

		pdat[i++] = ( (((DH&0x1C00)>>5) | ((DH&0x00F8)>>3)) << 8) | ((DL&0x1C00)>>5) | ((DL&0x00F8)>>3);
	}
	DELAY_DAT;
#endif
}

static void SET_MPULCD_TIME(unsigned short tSetup, unsigned short tHold, unsigned short tAcc)
{
	*(volatile unsigned short*)(SYNCGENMPUTTIME0) = (tSetup&0xFF)<<8 | (tHold&0xFF);
	*(volatile unsigned short*)(SYNCGENMPUTTIME1) = (tAcc&0xFF);
}

static void SET_SYNCGEN_CTRL(
		unsigned int FORMAT,
		unsigned int DITHERENB,
		unsigned int INTERRUTENB,
		unsigned int LCDTYPE	// 1: MPU TYPE
		)
{
	unsigned int	R_Dither, G_Dither, B_Dither;
	unsigned int	DELAY;
	unsigned int	YCRANGE;
	unsigned int	RGBMODE;
	unsigned int	SCANMODE;
	unsigned int	SENVENB;
	unsigned int	PADCLKSEL, PADCLKSEL1;
	unsigned int	CLKDV1;
	int RGB_6_B = FORMAT & RGBAB_MASK ? 0 : 1;	// B = 1, A = 0

	FORMAT = (FORMAT & FORMAT_MASK);

	R_Dither = 0;
	G_Dither = 0;
	B_Dither = 0;
	RGBMODE	 = 1;
	SCANMODE = 0;
	SENVENB  = 0;
	CLKDV1 	 = 0;

	if (DITHERENB) {
		if ((FORMAT == RGB555) || (FORMAT == MRGB555A) || (FORMAT == MRGB555B)) {
			R_Dither = 2;
			G_Dither = 2;
			B_Dither = 2;
		} else if ((FORMAT == RGB666) || (FORMAT == MRGB666)) {
			R_Dither = 3;
			G_Dither = 3;
			B_Dither = 3;
		} else if ((FORMAT == RGB565) || (FORMAT == MRGB565)) {
			R_Dither = 2;
			G_Dither = 3;
			B_Dither = 2;
		}
	}

	PADCLKSEL1 = 0;
	if (LCDTYPE) {
		CLKDV1    = 1;
		DELAY     = 14;
		RGBMODE	  = 1;
		PADCLKSEL = 1;
	} else {
		if ((FORMAT == RGB555) || (FORMAT == RGB565) || (FORMAT == RGB666) || (FORMAT == RGB888)) {
			DELAY = 7;		PADCLKSEL = 0;
		} else if (FORMAT == SRGB888) {
			DELAY 	   = 42;
			SCANMODE   = 0;
			PADCLKSEL  = 0;
			PADCLKSEL1 = 1;
			CLKDV1 	   = 5;
		} else if (FORMAT == SRGBD8888) {
			DELAY     = 28;
			PADCLKSEL = 1;
			CLKDV1    = 3;
		} else if (FORMAT == CCIR601_16A) {
			RGBMODE	  = 0;
			PADCLKSEL = 0;
			DELAY 	  = 6;
		} else if ((FORMAT == CCIR656) || (FORMAT == CCIR601_8) || (FORMAT == CCIR601_16B)) {
			RGBMODE	  = 0;
			PADCLKSEL = 1;
			CLKDV1    = 1;
			DELAY     = 12;
		} else if ((1 == LCDTYPE) && (FORMAT == MRGB565)) {
			CLKDV1    = 1;
			DELAY     = 14;
			RGBMODE	  = 1;
			PADCLKSEL = 1;
		} else if (1 == LCDTYPE) {
			CLKDV1    = 1;
			DELAY     = 14;
			RGBMODE	  = 1;
			PADCLKSEL = 1;
		} else {
			CLKDV1    = 1;
			DELAY     = 14;
			RGBMODE	  = 0;
			PADCLKSEL = 1;
		}
	}
	YCRANGE = 0;

	if (FORMAT == CCIR656) {
		YCRANGE = 0;
		SENVENB = 1;
	}

	*(volatile unsigned short*)(SYNCGENCTRL0) = (RGBMODE<<12) | (INTERRUTENB<<11) | (SENVENB<<8) | (0<<4) | (SCANMODE<<3);
	*(volatile unsigned short*)(SYNCGENCTRL1) = (YCRANGE<<13) | (FORMAT<<8) | (B_Dither<<4) | (G_Dither<<2) | (R_Dither<<0);
	*(volatile unsigned short*)(SYNCGENCTRL2) = (PADCLKSEL1<<1) | PADCLKSEL | (RGB_6_B)<<4;
	*(volatile unsigned short*)(SYNCGENDELAY0) = (DELAY<<8) | DELAY;
	*(volatile unsigned short*)(SYNCGENDELAY1) = DELAY;

	#if (0)
	printf("SYNCGENCTRL0 = 0x%08x :0x%08x\n", SYNCGENCTRL0 , *(volatile unsigned short*)(SYNCGENCTRL0));
	printf("SYNCGENCTRL1 = 0x%08x :0x%08x\n", SYNCGENCTRL1 , *(volatile unsigned short*)(SYNCGENCTRL1));
	printf("SYNCGENCTRL2 = 0x%08x :0x%08x\n", SYNCGENCTRL2 , *(volatile unsigned short*)(SYNCGENCTRL2));
	printf("SYNCGENDELAY0= 0x%08x :0x%08x\n", SYNCGENDELAY0, *(volatile unsigned short*)(SYNCGENDELAY0));
	printf("SYNCGENDELAY1= 0x%08x :0x%08x\n", SYNCGENDELAY1, *(volatile unsigned short*)(SYNCGENDELAY1));
	printf("CLKGEN0DVOL = 0x%08x :0x%08x, CLKDIV0=%d\n", CLKGEN0DVOL, (0x02) | ((DPC_CLKDIV0-1)<<5) | (1<<2), DPC_CLKDIV0);
	printf("CLKGEN0DVOL = 0x%08x :0x%08x, CLKDV1=%d\n", CLKGEN1DVOL, (0x1E) | CLKDV1<<5 | (1<<2), CLKDV1);
	#endif

	// Set clock divior
	CLKDV1 = DPC_CLKDIV1;
	*(volatile unsigned short*)(CLKGEN0DVOL) = (DPC_OUTCLKINV0 <<1) | ((DPC_CLKDIV0-1)<<5) | (1<<2);	//doriya
	*(volatile unsigned short*)(CLKGEN1DVOL) = (0x1E) | CLKDV1<<5;
}

static void SET_SYNCGEN_TIME(
 		unsigned int HBP,
 		unsigned int HSW,
 		unsigned int HFP,
 		unsigned int HAW,
 		unsigned int VBP,
 		unsigned int VSW,
 		unsigned int VFP,
 		unsigned int VAH,
 		unsigned int FILTERENB,
 		unsigned int SOURCEWIDTH
 		)
{
	unsigned int SCALEENB;
	unsigned int SCALE;
	unsigned int HTOTAL, HSBEGIN, HABEGIN, HAEND;
	unsigned int VTOTAL, VSBEGIN, VABEGIN, VAEND;

	SCALEENB = 0;
	HTOTAL	= HBP + HSW + HFP + HAW -1;
	HSBEGIN	= HSW -1;
	HABEGIN	= HBP + HSW -1;
	HAEND	= HBP + HSW + HAW -1;

	VTOTAL	= VBP + VSW + VFP + VAH -1;
	VSBEGIN	= VSW -1;
	VABEGIN	= VBP + VSW -1;
	VAEND	= VBP + VSW + VAH -1;

	if (HAW != SOURCEWIDTH)	{
		SCALEENB = 1;
		if (FILTERENB == 1)
			SCALE = ((SOURCEWIDTH-1)<<11)/(HAW-1);
		else
			SCALE = ((SOURCEWIDTH)*2048)/(HAW);
	} else {
		SCALE = 0x800;
	}

	*(volatile unsigned short*)(SYNCGENHSCALEL) = (SCALE&0xff)<<8 | ((SCALEENB&FILTERENB)<<1) | SCALEENB;
	*(volatile unsigned short*)(SYNCGENHSCALEH) = (SCALE&0xff00)>>8;
	*(volatile unsigned short*)(SYNCGENSOURCEWIDTH) = SOURCEWIDTH-1;

	*(volatile unsigned short*)(SYNCGENTOTHLENG) = HTOTAL;
	*(volatile unsigned short*)(SYNCGENHSBEGIN ) = HSBEGIN;
	*(volatile unsigned short*)(SYNCGENHABEGIN ) = HABEGIN;
	*(volatile unsigned short*)(SYNCGENHAEND   ) = HAEND;
	*(volatile unsigned short*)(SYNCGENTOTVLENG) = VTOTAL;
	*(volatile unsigned short*)(SYNCGENVSBEGIN ) = VSBEGIN;
	*(volatile unsigned short*)(SYNCGENVABEGIN ) = VABEGIN;
	*(volatile unsigned short*)(SYNCGENVAEND   ) = VAEND;
}

#if (0)
static void DUMP_LCD_REG(unsigned int cmd, unsigned int *data, int len)
{
	int i = 0;
	printf(" CMD = 0x%04x\n", cmd);
	MPU_READ_CD(cmd, data, len);
	for (i = 0; len > i; i++)
		printf("[%2d] = 0x%04x\n", i, data[i]);
}
#endif

/*----------------------------------------------------------------------------*/
#define	RGB_FORMAT		RGB565A

static void mpu_lcd_init(void)
{
	unsigned int cmd, len, dat[100] = {0, };

	SET_MPULCD_TIME  (0xf, 0xf, 0xf);
	SET_SYNCGEN_CTRL (RGB_FORMAT, 1, 0, 1);	// interrupt disable
	SET_SYNCGEN_TIME (tHBP, tHSW, tHFP, tHACT, tVBP, tVSW, tVFP, tVACT, 0,tHACT );

	/*
	 *	Sleep Out
	 */
	MPU_COMMAND(0x0011);
	udelay(80*1000);

	cmd = 0x00D3;
	len = 4;
	MPU_READ_CD(cmd,  dat, len);
	if (0x0094 != dat[2] || 0x0086 != dat[3]) {
		printf("0x%2x,%2x is invalid ILI9486 ID (0x0094, 0x0086)...\n",
			dat[2], dat[3]);
		return;
	}

	MPU_COMMAND(0x00B1);
	MPU_WRITE_D(0x00D0);	// 0x00D0
	MPU_WRITE_D(0x0011);

	MPU_COMMAND(0x00B4);
	MPU_WRITE_D(0x0000);	// 0x0002

	MPU_COMMAND(0x00B6);
	MPU_WRITE_D(0x0000);
	MPU_WRITE_D(0x0042);	// 0x0042
	MPU_WRITE_D(0x003B);

	MPU_COMMAND(0x00B7);
	MPU_WRITE_D(0x0007);

	MPU_COMMAND(0x00C0);
	MPU_WRITE_D(0x000C);
	MPU_WRITE_D(0x000C);

	MPU_COMMAND(0x00C1);
	MPU_WRITE_D(0x0045);
	MPU_WRITE_D(0x0000);

	MPU_COMMAND(0x00C2);
	MPU_WRITE_D(0x0033);

	MPU_COMMAND(0x00C5);
	MPU_WRITE_D(0x0000);
	MPU_WRITE_D(0x0024);
	MPU_WRITE_D(0x0080);

	MPU_COMMAND(0x00E0);
	MPU_WRITE_D(0x001F);
	MPU_WRITE_D(0x0025);
	MPU_WRITE_D(0x0022);
	MPU_WRITE_D(0x000B);
	MPU_WRITE_D(0x0006);
	MPU_WRITE_D(0x000A);
	MPU_WRITE_D(0x004E);
	MPU_WRITE_D(0x00C6);
	MPU_WRITE_D(0x0038);
	MPU_WRITE_D(0x0009);
	MPU_WRITE_D(0x0018);
	MPU_WRITE_D(0x0007);
	MPU_WRITE_D(0x000D);
	MPU_WRITE_D(0x0009);
	MPU_WRITE_D(0x0000);

	MPU_COMMAND(0x00E1);
	MPU_WRITE_D(0x000F);
	MPU_WRITE_D(0x0036);
	MPU_WRITE_D(0x0032);
	MPU_WRITE_D(0x0008);
	MPU_WRITE_D(0x000E);
	MPU_WRITE_D(0x0006);
	MPU_WRITE_D(0x0047);
	MPU_WRITE_D(0x0049);
	MPU_WRITE_D(0x0031);
	MPU_WRITE_D(0x0005);
	MPU_WRITE_D(0x0009);
	MPU_WRITE_D(0x0003);
	MPU_WRITE_D(0x001C);
	MPU_WRITE_D(0x001A);
	MPU_WRITE_D(0x0000);

	MPU_COMMAND(0x0036);
	MPU_WRITE_D(0x00C8); // video page use 0x0068

	MPU_COMMAND(0x003A);
	MPU_WRITE_D(0x0055); // 55 05

	MPU_COMMAND(0x002A);
    MPU_WRITE_D(0x0000);		// ??????
    MPU_WRITE_D(0x0000);
    MPU_WRITE_D(0x0001);
	MPU_WRITE_D(0x003F);

	MPU_COMMAND(0x002B);
	MPU_WRITE_D(0x0000);		// ??????
	MPU_WRITE_D(0x0000);
	MPU_WRITE_D(0x0001);
	MPU_WRITE_D(0x00EF);

	udelay(250*1000);

	/*
	 *	Display On
	 */
	MPU_COMMAND(0x0029);

	/*
 	 * Command : write to LCD
 	 */
	MPU_COMMAND(0X002C);

	/*
 	 * Sync Enable : write to LCD
 	 */
	*(volatile unsigned int*)(CLKGENENB) = *(volatile unsigned int*)(CLKGENENB) | (1<<2);
	*(volatile unsigned int*)(SYNCGENCTRL0) = *(volatile unsigned int*)(SYNCGENCTRL0) | (1<<15);
}

