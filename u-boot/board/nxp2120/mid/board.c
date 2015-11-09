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
#include <mmc.h>

/* nexell soc headers */
#include <platform.h>
#include <platfunc.h>

DECLARE_GLOBAL_DATA_PTR;

#include "eth.c"

/* debug macro */
#define DBGOUT(msg...)		{ printf(msg); }

/*------------------------------------------------------------------------------
 * intialize nexell soc and board status.
 */
static void init_gpio_pad(void);
static void init_alive_pad(void);
static void init_bus_pad(void);
static void init_display(int dev);
#ifdef CONFIG_CHECK_POWERKEY
static void init_power_key(void);
static void power_valid(void);
#endif

/* swpark add for upgrade, diagnostic boot check */
#ifdef CONFIG_CHECKBOOT
extern int run_command (const char *cmd, int flag);

#define CFG_PIO_CHECK_UPGRADE (PAD_GPIO_C + 16) // menu key
static int check_gpio(void)
{
	int grp = (CFG_PIO_CHECK_UPGRADE >> 5);
	int bit = (CFG_PIO_CHECK_UPGRADE & 0x1F);
	return !NX_GPIO_GetInputValue (grp, bit);
}

static int check_boot(void)
{
	int ret;

	ret = run_command("mmc part 0", 0);
	if (ret < 0) {
		printf("%s: mmc part 0 error\n", __func__);
		goto ERR_OUT;
	}

	ret = run_command("fatls mmc 0 boot/uImage", 0);
	if (ret < 0) {
		printf("%s: fatls boot/uImage error\n", __func__);
		goto ERR_OUT;
	}

	ret = run_command("fatls mmc 0 boot/rootfs.img.gz", 0);
	if (ret < 0) {
		printf("%s: fatls boot/rootfs.img.gz error\n", __func__);
		goto ERR_OUT;
	}

	ret = 0;

ERR_OUT:
	return ret;
}

static void boot_from_sd(void)
{
	int ret;
	char *rootfs_size = NULL;

#ifdef CFG_BOOT_FROM_USB
	ret = run_command("fatload mmc 0 81000000 boot/usbboot.mbr", 0);
	if (ret < 0) {
		printf("%s: fatload boot/usbboot.mbr error\n", __func__);
		return;
	}

	ret = run_command("usb write 81000000 0 1", 0);
	if (ret < 0) {
		printf("%s: usb write 81000000 0 1 error\n", __func__);
		return;
	}
#endif

	ret = run_command("fatload mmc 0 81000000 boot/uImage", 0);
	if (ret < 0) {
		printf("%s: fatload boot/uImage error\n", __func__);
		return;
	}

	ret = run_command("fatload mmc 0 88000000 boot/rootfs.img.gz", 0);
	if (ret < 0) {
		printf("%s: fatload boot/rootfs.img.gz error\n", __func__);
		return;
	}

	rootfs_size = getenv("filesize");
	if (rootfs_size) {
		ret = simple_strtoul(rootfs_size, NULL, 16);
		printf("rootfs size : %d\n", ret);
		if (ret <= 3600000) {
			setenv("bootargs", "root=/dev/ram0 rootfstype=ext2 ramdisk_size=8192 initrd=0x88000000,8M console=ttyS0,115200");
		} else {
			setenv("bootargs", "root=/dev/ram0 rootfstype=ext2 ramdisk_size=32768 initrd=0x88000000,32M console=ttyS0,115200");
		}
	}

	setenv("bootcmd", "bootm 81000000");
}
#endif

extern int do_usb(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);

#ifdef CONFIG_CHECK_USB_STORAGE_NUM
static inline int check_usb_stor_partition(block_dev_desc_t *stor_dev, int expected_num)
{
	int i;
	int ret = 0;
	disk_partition_t info;
	for (i = 1; i <= expected_num; i++) {
		ret = get_partition_info(stor_dev, i, &info);
		if (ret < 0) return ret;
	}
	return 0;
}

static int check_boot_usb_storage(int num)
{
	block_dev_desc_t *stor_dev;
	stor_dev = usb_stor_get_dev(num);
	if (stor_dev != NULL) {
		return check_usb_stor_partition(stor_dev, 4);
	}

	return -1;
}

static void set_default_usb_storage(int boot_device_num)
{
	char *cmd[] = { "usb", "dev", "" };
	char dev_num_str[2] = { 0 };
	sprintf(dev_num_str, "%d", boot_device_num);
	cmd[2] = dev_num_str;
	do_usb(NULL, 0, 3, cmd);
}

static void config_usb_bootargs(int boot_device_num)
{
	switch(boot_device_num) {
	case 0:
		setenv("bootargs", "root=/dev/sda1 rw rootwait rootfstype=ext4 console=ttyS0,115200n8 androidboot.hardware=nexell androidboot.console=ttyS0 init=/init raid=noautodetect");
		break;
	case 1:
		setenv("bootargs", "root=/dev/sdb1 rw rootwait rootfstype=ext4 console=ttyS0,115200n8 androidboot.hardware=nexell androidboot.console=ttyS0 init=/init raid=noautodetect");
		break;
	}
}
#endif

static inline void external_hub_reset(void)
{
	int grp = (CFG_PIO_USB_HUB_RESET >> 5);
	int bit = (CFG_PIO_USB_HUB_RESET & 0x1F);
	NX_GPIO_SetOutputValue (grp, bit, CFALSE);	mdelay(1);
	NX_GPIO_SetOutputValue (grp, bit, CTRUE);	mdelay(1);
}

static inline void external_hub_disable(void)
{
	int grp = (CFG_PIO_USB_HUB_RESET >> 5);
	int bit = (CFG_PIO_USB_HUB_RESET & 0x1F);
	NX_GPIO_SetOutputValue (grp, bit, CFALSE);
}

static inline void external_hub_enable(void)
{
	int grp = (CFG_PIO_USB_HUB_RESET >> 5);
	int bit = (CFG_PIO_USB_HUB_RESET & 0x1F);
	NX_GPIO_SetOutputValue (grp, bit, CTRUE);
	mdelay(1);
}

int board_init(void)
{
	init_gpio_pad();
	init_alive_pad();
	init_bus_pad();
#ifdef CONFIG_CHECK_POWERKEY
	init_power_key();
#endif

	external_hub_reset();

	DBGOUT("%s : done board initialize ...\n", CFG_SYS_BOARD_NAME);
	return 0;
}

int board_late_init(void)
{
	printf("Name : %s \n\n", CFG_SYS_BOARD_NAME);
#ifdef CFG_BOOT_FROM_USB
	char *cmd[] = { "usb", "start" };

	#ifdef CONFIG_CHECK_POWERKEY
	power_valid();
	#endif

	#ifdef CFG_PIO_VIBRATION_ENABLE
	{
		int grp = (CFG_PIO_VIBRATION_ENABLE >> 5);
		int bit = (CFG_PIO_VIBRATION_ENABLE & 0x1F);
		NX_GPIO_SetOutputValue (grp, bit, CTRUE);
		mdelay(200);
		NX_GPIO_SetOutputValue (grp, bit, CFALSE);
	}
	#endif

	#ifdef CONFIG_CMD_USB_LOWLEVEL_INIT
	{
		char *cmd_init[] = { "usb", "init" };
		do_usb(NULL, 0, 2, cmd_init);
		// delay
		mdelay(200);
	}
	#endif

	if (do_usb(NULL, 0, 2, cmd)) {
		/* Reset */
		printf("@@@ %s: Fail USB HUB -> CPU RESET @@@\n", __func__);
	    NX_CLKPWR_SetSoftwareResetEnable(CTRUE);
	    NX_CLKPWR_DoSoftwareReset();
	}

	#ifdef CONFIG_CHECK_USB_STORAGE_NUM
	if (check_boot_usb_storage(0)) {
		// 0 is not usb boot storage
		if (check_boot_usb_storage(1)) {
			// 1 is not usb boot storage
			// error
			printf("[FATAL ERROR] Check USB Storage!!!!\n");
			return -1;
		} else {
			set_default_usb_storage(1);
			config_usb_bootargs(1);
		}
	}
	#endif

#endif // CFG_BOOT_FROM_USB

#if defined(CONFIG_BOOTLOGO)
	#ifdef CONFIG_GENERIC_MMC
	mmc_initialize(gd->bd);
	#endif

	extern int load_boot_logo(void);
	load_boot_logo();
#endif

	init_display(CFG_DISP_MAIN_SCREEN);

	/* swpark add for checking upgrade, diagnostic booting */
#ifdef CONFIG_CHECKBOOT
	if(!check_boot()) {
	//if(check_gpio() && !check_boot()) {
		boot_from_sd();
	} else {
#ifdef CFG_BOOT_FROM_NAND
		setenv("bootcmd", "nand read 81000000 0 270000\;nand read 88000000 a00000 25738\;bootm 81000000");
#endif
	}
#endif
	return 0;
}

void board_preboot_os(void)
{
	int grp = (CFG_PIO_USB_HUB_RESET >> 5);
	int bit = (CFG_PIO_USB_HUB_RESET & 0x1F);

	printf("Preboot kernel ...\n");

	// Reset USB hub : Active Low
	NX_GPIO_SetPadFunction (grp, bit, NX_GPIO_PADFUNC_GPIO);
	NX_GPIO_SetOutputEnable(grp, bit, CTRUE);
	NX_GPIO_SetOutputValue (grp, bit, CFALSE);
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
extern void nx_boot_logo(U32 framebase, int x_resol, int y_resol, U32 pixelbyte);

/*----------------------------------------------------------------------------*/
struct pwm_pad {
	unsigned int group;
	unsigned int bit;
	unsigned int func;
};

static struct pwm_pad pwm_ch[] = {
	{ PAD_GET_GRP(PAD_GPIO_B),  2, NX_GPIO_PADFUNC_1 },
	{ PAD_GET_GRP(PAD_GPIO_B),  3, NX_GPIO_PADFUNC_1 }
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
/*-----------------------------------------------------------------------------*/

static void init_display(int dev)
{
	volatile int delay = 0;

	NX_DPC_DITHER RDither, GDither, BDither;

	U32  DPCFormat = CFG_DISP_PRI_OUT_FORMAT;
  	bool bEmbSync  = CFALSE;
    U32  XResol	   = CFG_DISP_PRI_RESOL_WIDTH;
    U32  YResol	   = CFG_DISP_PRI_RESOL_HEIGHT;
	U32	 LockSize  = CFG_DISP_PRI_MLC_LOCKSIZE;

	U32  PFrameBase= (CFG_MEM_PHY_BLOCK_BASE ? CFG_MEM_PHY_BLOCK_BASE : CFG_MEM_PHY_LINEAR_BASE);
	U32  VFrameBase= (U32)IO_ADDRESS(PFrameBase);
	if ((dev == 0 && CFALSE == CFG_DISP_PRI_BOOT_ENB))
		return;

	//--------------------------------------------------------------------------
	// LCD Off
	//--------------------------------------------------------------------------
#if	(CFG_LCD_PRI_LCD_ON)
	if (dev == 0) {
		NX_GPIO_SetOutputValue(PAD_GET_GRP(CFG_PIO_LCD_PCI_ENB), PAD_GET_BIT(CFG_PIO_LCD_PCI_ENB), CFALSE);
		for (delay=0; 100 > delay; delay++) { ; }
	}
#endif

	//--------------------------------------------------------------------------
	// DPC Clock Disalbe
	//--------------------------------------------------------------------------
	NX_DPC_SetClockPClkMode(dev, NX_PCLKMODE_DYNAMIC);
	NX_DPC_SetDPCEnable(dev, CFALSE);
	NX_DPC_SetClockDivisorEnable(dev, CFALSE);

	//--------------------------------------------------------------------------
	// Horizontla Up scale
	// Note: Only seconary DPC can scale up of horizontal width.
	//--------------------------------------------------------------------------

	//--------------------------------------------------------------------------
	// Set DPC
	//--------------------------------------------------------------------------
	// Dithering
    if (((U32)NX_DPC_FORMAT_RGB555   == DPCFormat) ||
		((U32)NX_DPC_FORMAT_MRGB555A == DPCFormat) ||
		((U32)NX_DPC_FORMAT_MRGB555B == DPCFormat))
	{
		RDither = GDither = BDither = NX_DPC_DITHER_5BIT;
	}
	else if (((U32)NX_DPC_FORMAT_RGB565  == DPCFormat) ||
			 ((U32)NX_DPC_FORMAT_MRGB565 == DPCFormat))
	{
		RDither = BDither = NX_DPC_DITHER_5BIT;
		GDither           = NX_DPC_DITHER_6BIT;
	}
	else if (((U32)NX_DPC_FORMAT_RGB666  == DPCFormat) ||
			 ((U32)NX_DPC_FORMAT_MRGB666 == DPCFormat))
	{
		RDither = GDither = BDither = NX_DPC_DITHER_6BIT;
	}
	else
	{
		RDither = GDither = BDither = NX_DPC_DITHER_BYPASS;
	}

	//--------------------------------------------------------------------------
	// VCLK2 : CLKGEN0
	NX_DPC_SetClockSource  (dev, 0, CFG_DISP_PRI_CLKGEN0_SOURCE);	// CLKSRCSEL
	NX_DPC_SetClockDivisor (dev, 0, CFG_DISP_PRI_CLKGEN0_DIV);		// CLKDIV
	NX_DPC_SetClockOutDelay(dev, 0, CFG_DISP_PRI_CLKGEN0_DELAY); 	// OUTCLKDELAY

	// VCLK : CLKGEN1
	NX_DPC_SetClockSource  (dev, 1, CFG_DISP_PRI_CLKGEN1_SOURCE);	// CLKSRCSEL  : CLKGEN0's out
	NX_DPC_SetClockDivisor (dev, 1, CFG_DISP_PRI_CLKGEN1_DIV);		// CLKDIV
	NX_DPC_SetClockOutDelay(dev, 1, CFG_DISP_PRI_CLKGEN1_DELAY); 	// OUTCLKDELAY

	//--------------------------------------------------------------------------
	NX_DPC_SetMode( dev,
					  DPCFormat,						// FORMAT
				 	  CFG_DISP_PRI_OUT_INTERLACE,		// SCANMODE
	             	  CFG_DISP_PRI_OUT_POL_INVERT,   	// POLFIELD
	             	  CFG_DISP_PRI_OUT_RGB, 			// RGBMODE
	             	  CFALSE,       					// SWAPRB
	             	  CFG_DISP_PRI_OUT_YCORDER,			// YCORDER
	             	  bEmbSync,							// YCCLIP
	             	  bEmbSync,  						// Embedded sync, Note> have to set to CTRUE for ITU-R BT.656
	             	  CFG_DISP_PRI_PADCLKSEL,			// PADCLKSEL
	             	  CFG_DISP_PRI_OUT_CLK_INVERT,		// PADCLKINV
					  CFG_DISP_PRI_OUT_DUAL_VIEW);

	NX_DPC_SetHSync(dev,
					  XResol,
					  CFG_DISP_PRI_HSYNC_SYNC_WIDTH,
					  CFG_DISP_PRI_HSYNC_FRONT_PORCH,
					  CFG_DISP_PRI_HSYNC_BACK_PORCH,
					  CFG_DISP_PRI_HSYNC_ACTIVE_HIGH);

	NX_DPC_SetVSync(dev,
					  YResol,
					  CFG_DISP_PRI_VSYNC_SYNC_WIDTH,
					  CFG_DISP_PRI_VSYNC_FRONT_PORCH,
					  CFG_DISP_PRI_VSYNC_BACK_PORCH,
					  CFG_DISP_PRI_VSYNC_ACTIVE_HIGH,
					  CFG_DISP_PRI_EVSYNC_ACTIVE_HEIGHT,
					  CFG_DISP_PRI_EVSYNC_SYNC_WIDTH,
					  CFG_DISP_PRI_EVSYNC_FRONT_PORCH,
					  CFG_DISP_PRI_EVSYNC_BACK_PORCH);

	NX_DPC_SetVSyncOffset(dev,
						CFG_DISP_PRI_VSYNC_START_OFFSET,
						CFG_DISP_PRI_VSYNC_END_OFFSET,
						CFG_DISP_PRI_EVSYNC_START_OFFSET,
						CFG_DISP_PRI_EVSYNC_END_OFFSET);

	NX_DPC_SetDelay(dev,
					  CFG_DISP_PRI_SYNC_DELAY_RGB_PVD,		// DELAYRGB
	             	  CFG_DISP_PRI_SYNC_DELAY_HS_CP1,		// DELAYHS_CP1
	             	  CFG_DISP_PRI_SYNC_DELAY_VS_FRAM,		// DELAYVS_FRAM
	             	  CFG_DISP_PRI_SYNC_DELAY_DE_CP2);		// DELAYDE_CP2

   	NX_DPC_SetDither(dev, RDither, GDither, BDither);

	NX_DPC_SetDPCEnable		 (dev, CTRUE);
	NX_DPC_SetClockDivisorEnable(dev, CTRUE);			// CLKENB : Provides internal operating clock.

	NX_DPC_SetMode( dev,
					  DPCFormat,						// FORMAT
					  CFG_DISP_PRI_OUT_INTERLACE,		// SCANMODE
	             	  CFG_DISP_PRI_OUT_POL_INVERT,   	// POLFIELD
	             	  CFG_DISP_PRI_OUT_RGB, 			// RGBMODE
	             	  CFALSE,       					// SWAPRB
	             	  CFG_DISP_PRI_OUT_YCORDER,			// YCORDER
	             	  bEmbSync,							// YCCLIP
	             	  bEmbSync,  						// Embedded sync, Note> have to set to CTRUE for ITU-R BT.656
	             	  CFG_DISP_PRI_PADCLKSEL,			// PADCLKSEL
	             	  CFG_DISP_PRI_OUT_CLK_INVERT,		// PADCLKINV
					  CFG_DISP_PRI_OUT_DUAL_VIEW);

	for (delay=0; 100 > delay; delay++) { ; }

	//--------------------------------------------------------------------------
	// LCD On
	//--------------------------------------------------------------------------
#if	(CFG_LCD_PRI_LCD_ON)
	if (dev == 0)
		NX_GPIO_SetOutputValue(PAD_GET_GRP(CFG_PIO_LCD_PCI_ENB), PAD_GET_BIT(CFG_PIO_LCD_PCI_ENB), CTRUE);
#endif

	//--------------------------------------------------------------------------
	// Set DPC
	//--------------------------------------------------------------------------
	// RGB TOP Layer
	//
	NX_MLC_SetClockPClkMode	(dev, NX_PCLKMODE_DYNAMIC);
	NX_MLC_SetClockBClkMode	(dev, NX_BCLKMODE_DYNAMIC);

	NX_MLC_SetLayerPriority	(dev, CFG_DISP_LAYER_VIDEO_PRIORITY);
	NX_MLC_SetBackground   	(dev, CFG_DISP_BACK_GROUND_COLOR);
	NX_MLC_SetFieldEnable		(dev, CFG_DISP_PRI_MLC_INTERLACE);
	NX_MLC_SetScreenSize		(dev, XResol, YResol);

	NX_MLC_SetRGBLayerGamaTablePowerMode 	(dev, CFALSE, CFALSE, CFALSE);
	NX_MLC_SetRGBLayerGamaTableSleepMode 	(dev, CTRUE, CTRUE, CTRUE);
	NX_MLC_SetRGBLayerGammaEnable 			(dev, CFALSE);
	NX_MLC_SetDitherEnableWhenUsingGamma 	(dev, CFALSE);
	NX_MLC_SetGammaPriority 				(dev, CFALSE);

    NX_MLC_SetTopPowerMode		(dev, CTRUE);
    NX_MLC_SetTopSleepMode		(dev, CFALSE);
	NX_MLC_SetMLCEnable		(dev, CTRUE);

	NX_MLC_SetTopDirtyFlag		(dev);

	// RGB Layer
	//
	if (dev == 0 && CFALSE == CFG_DISP_PRI_BOOT_LOGO)
		return;

	NX_MLC_SetLockSize			(dev, CFG_DISP_LAYER_SCREEN, LockSize);

	NX_MLC_SetAlphaBlending 	(dev, CFG_DISP_LAYER_SCREEN, CFALSE, 15);
	NX_MLC_SetTransparency  	(dev, CFG_DISP_LAYER_SCREEN, CFALSE, 0);
	NX_MLC_SetColorInversion	(dev, CFG_DISP_LAYER_SCREEN, CFALSE, 0);

	NX_MLC_SetRGBLayerInvalidPosition(dev, CFG_DISP_LAYER_SCREEN, 0, 0, 0, 0, 0, CFALSE);
	NX_MLC_SetRGBLayerInvalidPosition(dev, CFG_DISP_LAYER_SCREEN, 1, 0, 0, 0, 0, CFALSE);

	NX_MLC_SetFormatRGB 		(dev, CFG_DISP_LAYER_SCREEN, CFG_DISP_SCREEN_RGB_FORMAT);
	NX_MLC_SetPosition 		(dev, CFG_DISP_LAYER_SCREEN, 0, 0, XResol-1, YResol-1);

printf("<0x%x, %d,%d, %d>\n", VFrameBase, XResol, YResol, CFG_DISP_SCREEN_PIXEL_BYTE);
	if (dev == 0)
		nx_boot_logo(VFrameBase, XResol, YResol, CFG_DISP_SCREEN_PIXEL_BYTE);

	if (dev == 1) {
		PFrameBase += (CFG_DISP_PRI_RESOL_WIDTH*CFG_DISP_PRI_RESOL_HEIGHT*CFG_DISP_SCREEN_PIXEL_BYTE);
		VFrameBase += (CFG_DISP_PRI_RESOL_WIDTH*CFG_DISP_PRI_RESOL_HEIGHT*CFG_DISP_SCREEN_PIXEL_BYTE);
		nx_boot_logo(VFrameBase, XResol, YResol, CFG_DISP_SCREEN_PIXEL_BYTE);
	}

	NX_MLC_SetRGBLayerStride(dev, CFG_DISP_LAYER_SCREEN, CFG_DISP_SCREEN_PIXEL_BYTE, XResol*CFG_DISP_SCREEN_PIXEL_BYTE);
	NX_MLC_SetRGBLayerAddress(dev, CFG_DISP_LAYER_SCREEN, PFrameBase);

	NX_MLC_SetLayerEnable(dev, CFG_DISP_LAYER_SCREEN, CTRUE);
	NX_MLC_SetDirtyFlag(dev, CFG_DISP_LAYER_SCREEN);

	//--------------------------------------------------------------------------
	// LCD Backlight On
	//--------------------------------------------------------------------------
	if (dev == 0 && CFG_LCD_PRI_BLU_ON == CTRUE) {
		int r_scale = 0, r_period = 0, r_duty = 0;

		NX_PWM_SetClockPClkMode(NX_PCLKMODE_ALWAYS);
		NX_PWM_SetClockDivisorEnable(CFALSE);
		NX_PWM_SetClockSource(0, PWM_CLOCK_SOURCE);
		NX_PWM_SetClockDivisor(0, PWM_CLOCK_DIVISOR);

		if (CFG_LCD_PRI_PWM_FREQ > PWM_FREQUENCY_MAX/PWM_PERIOD_MAX) {
			r_scale  = PWM_PRESCL_MIN;
			r_period = (PWM_FREQUENCY_MAX/r_scale)/CFG_LCD_PRI_PWM_FREQ;
		} else {
			r_period = PWM_PERIOD_MAX;
			r_scale  = (PWM_FREQUENCY_MAX/r_period)/CFG_LCD_PRI_PWM_FREQ;
		}
		r_duty = (r_period * CFG_LCD_PRI_PWM_DUTYCYCLE) / 100;

		NX_PWM_SetPreScale	(CFG_LCD_PRI_PWM_CH, r_scale);
		NX_PWM_SetPeriod	(CFG_LCD_PRI_PWM_CH, r_period);
		NX_PWM_SetDutyCycle	(CFG_LCD_PRI_PWM_CH, r_duty);

		NX_PWM_SetClockDivisorEnable(CTRUE);

		/* Enable pwm out pad*/
		NX_GPIO_SetPadFunction(pwm_ch[CFG_LCD_PRI_PWM_CH].group,
			pwm_ch[CFG_LCD_PRI_PWM_CH].bit, (NX_GPIO_PADFUNC)pwm_ch[CFG_LCD_PRI_PWM_CH].func);
	}
}

/*-----------------------------------------------------------------------------*/
#ifdef CONFIG_CHECK_POWERKEY

static void init_power_key(void)
{
	U32 pwrsrc[] = {
		CFG_PWR_POWER_CTL_ALIVE0, CFG_PWR_POWER_CTL_ALIVE1,
		CFG_PWR_POWER_CTL_ALIVE2, CFG_PWR_POWER_CTL_ALIVE3 };

	U32 detmod[] = {
		PWR_DECT_SYNC_HIGHLEVEL, PWR_DECT_SYNC_HIGHLEVEL,
		PWR_DECT_SYNC_HIGHLEVEL, PWR_DECT_SYNC_HIGHLEVEL };

	int i = 0, alvnum;
	alvnum = sizeof(pwrsrc)/sizeof(pwrsrc[0]);

	NX_ALIVE_ClearInterruptPendingAll();

	for (i = 0 ; alvnum > i; i++) {
		NX_ALIVE_SetDetectMode(NX_ALIVE_DETECTMODE_ASYNC_LOWLEVEL	, i, CFALSE);
		NX_ALIVE_SetDetectMode(NX_ALIVE_DETECTMODE_ASYNC_HIGHLEVEL	, i, CFALSE);
		NX_ALIVE_SetDetectMode(NX_ALIVE_DETECTMODE_SYNC_FALLINGEDGE	, i, CFALSE);
		NX_ALIVE_SetDetectMode(NX_ALIVE_DETECTMODE_SYNC_RISINGEDGE	, i, CFALSE);
		NX_ALIVE_SetDetectMode(NX_ALIVE_DETECTMODE_SYNC_LOWLEVEL	, i, CFALSE);
		NX_ALIVE_SetDetectMode(NX_ALIVE_DETECTMODE_SYNC_HIGHLEVEL	, i, CFALSE);
		NX_ALIVE_SetInterruptEnable(i, CFALSE);
		NX_ALIVE_SetDetectEnable(i, CFALSE);

		if (pwrsrc[i]) {
			NX_ALIVE_SetOutputEnable(i, CFALSE);
			NX_ALIVE_SetDetectMode(detmod[i], i, CTRUE);
			NX_ALIVE_SetDetectEnable(i, CTRUE);
			NX_ALIVE_SetInterruptEnable(i, CTRUE);
		}
	}
	NX_ALIVE_GetInterruptPendingNumber();
}

static void power_valid(void)
{
	int i = 0, pend = 0, delay = 0;

	U32 pwrsrc[] = {
		CFG_PWR_POWER_CTL_ALIVE0, CFG_PWR_POWER_CTL_ALIVE1,
		CFG_PWR_POWER_CTL_ALIVE2, CFG_PWR_POWER_CTL_ALIVE3 };

	U32 detmod[] = {
		CFG_PWR_POWER_MOD_ALIVE0, CFG_PWR_POWER_MOD_ALIVE1,
		CFG_PWR_POWER_MOD_ALIVE2, CFG_PWR_POWER_MOD_ALIVE2 };

	int alvnum = sizeof(pwrsrc)/sizeof(pwrsrc[0]);

	pend = NX_ALIVE_GetInterruptPendingNumber();

	for (i = 0 ; alvnum > i; i++) {
		if (pwrsrc[i] && i == pend)
			goto power_off;
	}
	printf("Boot power on\n");

	NX_ALIVE_SetInterruptEnableAll(CFALSE);
	NX_ALIVE_ClearInterruptPendingAll();
	return;

power_off:
	printf("Boot power off [ALIVE:%d,%d]\n", i, pend);

	NX_ALIVE_ClearInterruptPendingAll();

	/* set wake up source */
	for (i = 0 ; alvnum > i; i++) {
		NX_ALIVE_SetDetectMode(NX_ALIVE_DETECTMODE_ASYNC_LOWLEVEL	, i, CFALSE);
		NX_ALIVE_SetDetectMode(NX_ALIVE_DETECTMODE_ASYNC_HIGHLEVEL	, i, CFALSE);
		NX_ALIVE_SetDetectMode(NX_ALIVE_DETECTMODE_SYNC_FALLINGEDGE	, i, CFALSE);
		NX_ALIVE_SetDetectMode(NX_ALIVE_DETECTMODE_SYNC_RISINGEDGE	, i, CFALSE);
		NX_ALIVE_SetDetectMode(NX_ALIVE_DETECTMODE_SYNC_LOWLEVEL	, i, CFALSE);
		NX_ALIVE_SetDetectMode(NX_ALIVE_DETECTMODE_SYNC_HIGHLEVEL	, i, CFALSE);
		NX_ALIVE_SetInterruptEnable(i, CFALSE);
		NX_ALIVE_SetDetectEnable(i, CFALSE);

		if (pwrsrc[i]) {
			NX_ALIVE_SetOutputEnable(i, CFALSE);
			NX_ALIVE_SetDetectMode(detmod[i], i, CTRUE);
			NX_ALIVE_SetDetectEnable(i, CTRUE);
		}
	}
	NX_ALIVE_ClearInterruptPendingAll();

	NX_ALIVE_SetPadRetentionHold(NX_ALIVE_PADGROUP0, CFG_PWR_SLEEP_PAD_HOLD_GROUP0);
	NX_ALIVE_SetPadRetentionHold(NX_ALIVE_PADGROUP1, CFG_PWR_SLEEP_PAD_HOLD_GROUP1);
	NX_ALIVE_SetPadRetentionHold(NX_ALIVE_PADGROUP2, CFG_PWR_SLEEP_PAD_HOLD_GROUP2);

	/* sum delay */
	for (delay = 0 ; 0xFFFFF > delay; delay++ ) { ; }

	NX_ALIVE_SetVDDPWRON(CFALSE, CFALSE);
	NX_CLKPWR_GoStopMode();

	while(1);
	return;
}

#endif

