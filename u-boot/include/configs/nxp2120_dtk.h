/*
 * (C) Copyright 2010 Nexell Co.,
 * jung hyun kim<jhkim@nexell.co.kr>
 *
 * Configuation settings for the Nexell board.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the * GNU General Public License for more details.  *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/*-----------------------------------------------------------------------
 * nexell soc headers
 */
#ifndef	__ASM_STUB_PROCESSOR_H__
#include <platform.h>
#endif

/*-----------------------------------------------------------------------
 * allow to overwrite serial and ethaddr
 */
#define CONFIG_ENV_OVERWRITE

#define CONFIG_SYS_HUSH_PARSER			/* use "hush" command parser	*/
#ifdef 	CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT_HUSH_PS2	"> "

#endif

/*-----------------------------------------------------------------------
 * High Level Configuration Options
 * (easy to change)
 */
#undef  CONFIG_USE_IRQ		     						/* Not used: not need IRQ/FIQ stuff	*/
#define CONFIG_SYS_HZ	   				1000			/* decrementer freq: 1ms ticks */
#define CONFIG_ARCH_CPU_INIT
#define CONFIG_ARCH_MISC_INIT
#define BOARD_LATE_INIT

/*-----------------------------------------------------------------------
 * Physical Memory Map
 */
#define CONFIG_NR_DRAM_BANKS	   		1				/* nexell soc has 1 bank of dram */

/*
 * Size of malloc() pool
 */
#define CONFIG_SYS_MALLOC_LEN			(512*1024)		/* for Initiazle heap, fix 512K */
#define CONFIG_SYS_GBL_DATA_SIZE		128	     		/* size in bytes reserved for initial data */
#define CONFIG_SYS_HEAP_LEN				(2*1024*1024)	/* for system heap, for UBIFS 2M */
/*
 * select serial console configuration
 */
#define CONFIG_BAUDRATE		   			CFG_UART_DEBUG_BAUDRATE
#define CONFIG_SYS_BAUDRATE_TABLE	   	{ 9600, 19200, 38400, 57600, 115200 }

/*-----------------------------------------------------------------------
 * Command definition
 */
#define	CONFIG_CMD_BDI				/* board info	*/
#define	CONFIG_CMD_IMI				/* image info	*/
#define	CONFIG_CMD_BOOTD
#define	CONFIG_CMD_DHCP
#define	CONFIG_CMD_NET
#define	CONFIG_CMD_MEMORY
#define	CONFIG_CMD_PING
#define	CONFIG_CMD_RUN				/* run commands in an environment variable	*/
#define CONFIG_CMDLINE_EDITING		/* add command line history	*/

#define	CONFIG_CMDLINE_TAG			/* use bootargs commandline */

/*-----------------------------------------------------------------------
 * This must be included AFTER the definition of CONFIG_COMMANDS (if any)
 */
#include <config_cmd_default.h>

/* refer to common/env_common.c	*/
#if 1
#define CONFIG_BOOTDELAY	   			2
#define CONFIG_ZERO_BOOTDELAY_CHECK
#define CONFIG_ETHADDR		   			00:FA:45:4a:59:24
#define CONFIG_NETMASK		   			255.255.0.0
#define CONFIG_IPADDR					192.168.1.214
#define CONFIG_SERVERIP					192.168.1.33
#define CONFIG_GATEWAYIP				192.168.1.254
#define CONFIG_BOOTFILE					"uImage"  		/* File to load	*/

#define CONFIG_EXTRA_ENV_SETTINGS \
        "console=ttyS0\0" \
		"ethact=ax88796b\0" \
		"stdin=serial\0" \
		"stdout=serial\0" \
		"stderr=serial\0" \
		"ezb_load_r=nand read 0x82000000 0xc00000 0x8a0000\0" \
		"ezb_load_k=nand read 0x81000000 0x400000 0x2c0000\0" \
		"ezb_load_k_a=nand read 0x81000000 0xf800000 0x2c0000\0" \
		"bootargs=console=ttyS0,115200 root=/dev/ram0 rw initrd=0x82000000,12M ramdisk=24576 mac=12:2b:45:4a:59:24:56 earlyprintk\0" \
		"ezb_bootargs=setenv bootargs ${ezb_KCMD1} ${ezb_KCMD2} mac=${ethaddr} ${ezb_KCMD3} ${ezb_KCMD4} ${ezb_KCMD5}\0" \
		"app_bootargs=setenv bootargs ${ezb_KCMD1} ${app_KCMD2} mac=${ethaddr} ${ezb_KCMD3} ${ezb_KCMD4} ${ezb_KCMD5}\0" \
		"ezb_KCMD1=console=ttyS0,115200\0" \
		"ezb_KCMD2=root=/dev/ram0 rw initrd=0x82000000,12M ramdisk=24576\0" \
		"ezb_KCMD3=earlyprintk\0" \
		"ezb_KCMD4=\0" \
		"ezb_KCMD5=\0" \
		"app_KCMD2=root=/dev/mtdblock8 rw yaffs\0" \
		"ezb_dt=none\0" \
		"ezb_autoexec=/app/run.sh\0" \
		"ezb_uboot=uboot.nxp2120\0" \
		"ezb_uImage=uImage\0" \
		"ezb_ramdisk=ramdisk.wc-24M.gz\0" \
		"boot_mode=env\0" \
		"ezb_bootm=nand read 0x81000000 0x400000 0x2c0000; nand read 0x82000000 0xc00000 0x8a0000; bootm 0x81000000\0" \
		"app_bootm=nand read 0x81000000 0xf800000 0x2c0000; nand read 0x82000000 0xc00000 0x8a0000; bootm 0x81000000\0" \
		"bootcmd=run ezb_bootargs ezb_bootm"
#else
#define CONFIG_BOOTDELAY	   			2
#define CONFIG_ZERO_BOOTDELAY_CHECK
#define CONFIG_ETHADDR		   			12:2b:45:4a:59:24:56
#define CONFIG_NETMASK		   			255.255.255.0
#define CONFIG_IPADDR					192.168.1.214
#define CONFIG_SERVERIP					192.168.1.33
#define CONFIG_GATEWAYIP				192.168.1.254
#define CONFIG_BOOTFILE					"uImage"  		/* File to load	*/
#define CONFIG_BOOTCOMMAND				"tftp 0x81000000 uImage; bootm 0x81000000"
#endif

/*-----------------------------------------------------------------------
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_PROMPT				"nxp2120# "     						/* Monitor Command Prompt   */
#define CONFIG_SYS_LONGHELP				       									/* undef to save memory	   */
#define CONFIG_SYS_CBSIZE		   		1024 // 256		   								/* Console I/O Buffer Size  */
#define CONFIG_SYS_PBSIZE		   		(CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16) /* Print Buffer Size */
#define CONFIG_SYS_MAXARGS			   	16		       							/* max number of command args   */
#define CONFIG_SYS_BARGSIZE			   	CONFIG_SYS_CBSIZE	       				/* Boot Argument Buffer Size    */

#define CONFIG_SYS_LOAD_ADDR			CFG_KERNEL_TEXT_BASE					/* default kernel load address */
#define CONFIG_SYS_MEMTEST_START		CFG_MEM_PHY_SYSTEM_BASE					/* memtest works on */
#define CONFIG_SYS_MEMTEST_END			CFG_MEM_PHY_SYSTEM_SIZE

/*-----------------------------------------------------------------------
 * Stack sizes
 *
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKSIZE		   		(256*1024)	  /* regular stack: 256K */

/*-----------------------------------------------------------------------
 * Ethernet drivers (DM9000 / W5300)
 */
#define	CONFIG_AX88796B

#ifdef CONFIG_DM9000
/* DM9000 */
#define CONFIG_DRIVER_DM9000			1
#define CONFIG_DM9000_BASE	   			CFG_EXT_PHY_BASEADDR_ETHER	/* DM9000: 0x10000000(CS4) */
#define DM9000_IO	   					CONFIG_DM9000_BASE
#define DM9000_DATA	   					(CONFIG_DM9000_BASE + 0x4)
//#define CONFIG_DM9000_DEBUG
#endif

#ifdef CONFIG_AX88796B
/* AX88796B */
#define CONFIG_DRIVER_AX88796B			1
#define AX88796B_BASE	   				CFG_EXT_PHY_BASEADDR_ETHER	/* AX88796B: 0x10000000(CS4) */
//#define CONFIG_AX88796B_DEBUG
#endif

#ifdef CONFIG_W5300
/* W5300 */
#define CONFIG_DRIVER_W5300				1
#define CONFIG_DRIVER_DEBUG				0							/* or 0 */
#define W5300_BASE						CFG_EXT_PHY_BASEADDR_ETHER	/* W5300: 0x0C000000(CS3) */
#define W5300_MAX_CHANNEL				2
#define W5300_TMSR_ARRAY				{32,32}
#define W5300_RMSR_ARRAY				{32,32}
#endif

#define CONFIG_NET_RETRY_COUNT			100
#define CONFIG_NET_MULTI				/* needed for eth.c (u-boot-09.11)*/

/*-----------------------------------------------------------------------
 * NAND FLASH
 */
#define	CONFIG_SYS_NO_FLASH
#define	CONFIG_CMD_NAND
#if !defined(CONFIG_CMD_NAND)

#define	CONFIG_SYS_NO_FLASH
#undef  CONFIG_CMD_IMLS					/* list all images found in flash	*/

#else	/* CONFIG_CMD_NAND */

#define CONFIG_SYS_64BIT_VSPRINTF		/* needed for nand_util.c (u-boot-09.11)*/

#define CONFIG_SYS_MAX_NAND_DEVICE		1

#define	CONFIG_SYS_NAND_BASE			PHY_BASEADDR_NAND_STATIC	/* Nand conrtoller base	*/
#define CONFIG_MTD_NAND_VERIFY_WRITE								/* set when software ecc mode */
#define CONFIG_SYS_NAND_HW_ECC
//#define CONFIG_CMD_NAND_MLC

#ifdef  CONFIG_CMD_NAND_MLC
#define	CONFIG_SYS_NAND_HW_ECC
#endif



/* etc configure macro */
#undef  CONFIG_CMD_IMLS											/* list all images found in flash	*/

/*-----------------------------------------------------------------------
 * NAND FLASH and environment organization
 */
//#define DEBUG_ENV
#define CONFIG_ENV_IS_IN_NAND									/* default: CONFIG_ENV_IS_NOWHERE */
#define	CONFIG_ENV_OFFSET				512*1024				/* 0x00080000 */
#define CONFIG_ENV_SIZE           		16*1024					/* 1 block size */
#define CONFIG_ENV_RANGE				CONFIG_ENV_SIZE * 4 	/* avoid bad block */

#endif	/* CONFIG_CMD_NAND */

/*-----------------------------------------------------------------------
 * Board specification configure
 */
#define CONFIG_MACH_NXP2120_PB_DTK

// psw0523 fix
#ifdef CONFIG_CMD_NAND
// for other nand write function
#define	CFG_NAND_WRITE_YAFFS2
#define	CFG_NAND_WRITE_BAD
#define	NAND_WRITE_BAD					(0x01)
#define	NAND_WRITE_YAFFS2				(0x02)
#endif

/*-----------------------------------------------------------------------
 * Wakeup functions
 */
#define	CONFIG_PM_WAKEUP
//#define	CONFIG_PM_WAKEUP_ASM

/*-----------------------------------------------------------------------
 * LCD config
 * support only 8/16 bpp LCD
 */
#if 0
#define	CONFIG_LCD
#define	CONFIG_LCD_LOGO
#define	CONFIG_LCD_INFO
#define	CONFIG_LCD_INFO_BELOW_LOGO

#define LCD_BPP							LCD_COLOR8	/* BMP file's depth */
#define CONFIG_WHITE_ON_BLACK			1
#define CONFIG_SPLASH_SCREEN			1
#define CONFIG_SYS_CONSOLE_IS_IN_ENV	1			/* set stdout to serial port */
#endif

/*-----------------------------------------------------------------------
 * UBI
 */
//#define CONFIG_CMD_UBIFS
#ifdef CONFIG_CMD_UBIFS

#define CONFIG_RBTREE
#define	CONFIG_CMD_UBI
#define	CONFIG_LZO

#if !defined(CONFIG_SYS_HEAP_LEN) || (2*1024*1024 > CONFIG_SYS_HEAP_LEN)
	#error "To UBI, heap length must be more than 2MB..."
#endif

//#define	CONFIG_UBIFS_FS_DEBUG
#ifdef  CONFIG_UBIFS_FS_DEBUG
#define	CONFIG_UBIFS_FS_DEBUG_MSG_LVL	1	/* For ubifs debug message = 0 ~ 3 */
#endif

#endif // CONFIG_CMD_UBIFS

/*-----------------------------------------------------------------------
 * MTD
 */
#if defined(CONFIG_CMD_UBIFS)
#define CONFIG_MTD_DEVICE
#define CONFIG_MTD_PARTITIONS
#define CONFIG_CMD_MTDPARTS

// align with block size,
// use: size@offset(name)
//
#define PART_BOOT			"2m(u-boot)ro,"
#define PART_KERNEL			"6m(kernel),"
#define PART_FS				"504m(fs)"

#define MTDIDS_DEFAULT		"nand0=nx_nand.0"
#define MTDPARTS_DEFAULT	"mtdparts=nx_nand.0:"	\
							PART_BOOT			\
							PART_KERNEL			\
							PART_FS

//#define CONFIG_MTD_DEBUG
#ifdef  CONFIG_MTD_DEBUG
#define CONFIG_MTD_DEBUG_VERBOSE		3	/* For nand debug message = 0 ~ 3 */
#endif

#endif

/*-----------------------------------------------------------------------
 * Extra env
 */
#ifndef MTDPARTS_DEFAULT
#define	MTDPARTS_DEFAULT
#endif

/*
#define CONFIG_EXTRA_ENV_SETTINGS	MTDPARTS_DEFAULT
*/

/*-----------------------------------------------------------------------
 * SDMMC
 */
#define CONFIG_CMD_MMC
#define CONFIG_MMC
#define	CONFIG_GENERIC_MMC
//#define CONFIG_ENV_IS_IN_MMC

/*
 * MMC ENV
 */
#if defined (CONFIG_GENERIC_MMC) && defined (CONFIG_ENV_IS_IN_MMC)
#undef  CONFIG_ENV_IS_IN_NAND
#undef  CONFIG_ENV_OFFSET
#undef  CONFIG_ENV_SIZE

#define CONFIG_CMD_SAVEENV
#define CONFIG_SYS_MMC_ENV_DEV  		0
#define CONFIG_MMC_ENV_BLOCK_OFFSET		2046
#define CONFIG_MMC_ENV_BLOCK_COUNT		2
#define CFG_MMC_BLOCK_SIZE				512
#define	CONFIG_ENV_OFFSET				(CFG_MMC_BLOCK_SIZE*CONFIG_MMC_ENV_BLOCK_OFFSET)
#define CONFIG_ENV_SIZE           		(CFG_MMC_BLOCK_SIZE*CONFIG_MMC_ENV_BLOCK_COUNT)
#endif

/*-----------------------------------------------------------------------
 * USB Host
 */
#define CONFIG_CMD_USB
#define CONFIG_USB_STORAGE
#define CONFIG_USB_EHCI
#define CONFIG_USB_EHCI_NXP2120
//#define CONFIG_USB_BOOT

/*
 * EEPROM
 */
//#define CONFIG_ENV_IS_IN_EEPROM
#define CONFIG_CMD_EEPROM
#define CONFIG_SYS_I2C_EEPROM_ADDR_LEN  1
#define CONFIG_CMD_I2C
#define CONFIG_SYS_I2C_SPEED        400000
#define CONFIG_SOC_I2C_NOSTOP       0


#if defined(CONFIG_USB_BOOT)
#undef CONFIG_ENV_IS_IN_NAND
#undef CONFIG_ENV_OFFSET
#undef CONFIG_BOOTCOMMAND
#define CONFIG_ENV_IS_NOWHERE
// swpark : for MID Board usb boot
//#define CONFIG_BOOTCOMMAND				"usb start\;fatload usb 0 81000000 uImage-usb.bin\;bootm 81000000"
#define CONFIG_BOOTCOMMAND				"usb start\;usb read 81000000 1 15ab\;bootm 81000000"
// MID Board new command : save image to usb
#define CONFIG_CMD_FAT_AND_SAVE_USB
#endif

/*-----------------------------------------------------------------------
   * CHECK BOOT : check upgrade boot vs normal boot for android
    */
#define CONFIG_CHECKBOOT

/*-----------------------------------------------------------------------
 * FAT
 */
#if defined(CONFIG_MMC) || defined(CONFIG_CMD_USB)
#define	CONFIG_DOS_PARTITION
#define	CONFIG_CMD_FAT
#endif

/*-----------------------------------------------------------------------
 * BOOTLOGO
 */
//#define CONFIG_BOOTLOGO_COMMAND			"nand read 8ec00000 600000 c0000; bootlogo 8ec00000"
#if defined(CONFIG_BOOTLOGO_COMMAND)
#define CONFIG_BOOTLOGO
#endif
//#define DEBUG_PARSER

/*-----------------------------------------------------------------------
 * Debug message
 */
#define	CONFIG_DISPLAY_CPUINFO			/* print_cpuinfo */
//#define CONFIG_UART_PRE_DEBUG			/* for previous boot message, before board_init */
//#define DEBUG							/* u-boot debug macro, nand, ethernet,... */
//#define CONFIG_PROTO_FUNC_DEBUG		/* nexell prototype debug mode */

#endif /* __CONFIG_H */

