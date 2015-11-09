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
 * Board specification configure
 */


/*-----------------------------------------------------------------------
 * nexell soc headers
 */
#ifndef	__ASM_STUB_PROCESSOR_H__
#include <platform.h>
#endif

#define CFG_SYS_BOARD_NAME	"nxp2120-info"

/*-----------------------------------------------------------------------
 * boot mode : exclusive
 */
//#define CFG_BOOT_FROM_NAND
//#define CFG_BOOT_FROM_USB

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
#define	CONFIG_CMD_MEMORY
#undef CONFIG_CMD_NET
#define	CONFIG_CMD_RUN				/* run commands in an environment variable	*/
#define CONFIG_CMDLINE_EDITING		/* add command line history	*/

#define	CONFIG_CMDLINE_TAG			/* use bootargs commandline */

/*-----------------------------------------------------------------------
 * This must be included AFTER the definition of CONFIG_COMMANDS (if any)
 */
#include <config_cmd_default.h>

/* refer to common/env_common.c	*/
#define CONFIG_BOOTDELAY	   			2
#define CONFIG_ZERO_BOOTDELAY_CHECK
#define CONFIG_ETHADDR		   			12:3b:41:5d:59:24:56
#define CONFIG_NETMASK		   			255.255.255.0
#define CONFIG_IPADDR					192.168.1.175
#define CONFIG_SERVERIP					192.168.1.33
#define CONFIG_GATEWAYIP				192.168.1.254
#define CONFIG_BOOTFILE					"uImage"  		/* File to load	*/

/*-----------------------------------------------------------------------
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_PROMPT				"nxp2120# "     						/* Monitor Command Prompt   */
#define CONFIG_SYS_LONGHELP				       									/* undef to save memory	   */
#define CONFIG_SYS_CBSIZE		   		256		   								/* Console I/O Buffer Size  */
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

/* DM9000 */
#define CONFIG_DRIVER_DM9000			1
#define CONFIG_DM9000_BASE	   			CFG_EXT_PHY_BASEADDR_ETHER	/* DM9000: 0x10000000(CS4) */
#define DM9000_IO	   					CONFIG_DM9000_BASE
#define DM9000_DATA	   					(CONFIG_DM9000_BASE + 0x4)

#define CONFIG_NET_RETRY_COUNT			100
#define CONFIG_NET_MULTI				/* needed for eth.c (u-boot-09.11)*/

/*-----------------------------------------------------------------------
 * EEPROM
 */

#define	CONFIG_SYS_NO_FLASH
#define	CONFIG_CMD_EEPROM

#if !defined(CONFIG_CMD_EEPROM)

#define	CONFIG_SYS_NO_FLASH
#undef  CONFIG_CMD_IMLS					/* list all images found in flash	*/
#define	CONFIG_ENV_IS_NOWHERE
#define CONFIG_ENV_SIZE					0x1000

#else	/* CONFIG_CMD_EEPROM */

#define CONFIG_SPI						/* SPI EEPROM, not I2C EEPROM */
#define CONFIG_SYS_64BIT_VSPRINTF		/* needed for nand_util.c (u-boot-09.11)*/

/* etc configure macro */
#undef  CONFIG_CMD_IMLS											/* list all images found in flash	*/

/*  EEPROM Environment Organization
 *	[Note R/W unit 64K]
 *
 *   0 ~   4K First Boot
 *   4 ~   8K NSIH
 *   8 ~ 248K U-Boot
 * 248 ~ 256K Environment
 */

//#define CONFIG_CMD_SAVEENV

#define CONFIG_ENV_IS_IN_EEPROM									/* default: CONFIG_ENV_IS_NOWHERE */
#define	CONFIG_ENV_OFFSET				248*1024
#define CONFIG_ENV_SIZE           		  8*1024
#define CONFIG_ENV_RANGE				CONFIG_ENV_SIZE
#define CONFIG_SYS_DEF_EEPROM_ADDR		0						/* Need 0, when SPI */
#define CONFIG_SYS_I2C_FRAM										/* To avoid max length limit when spi write */
//#define DEBUG_ENV

#define	CONFIG_1STBOOT_OFFSET			   0
#define	CONFIG_1STBOOT_SIZE				   4*1024
#define	CONFIG_NSIH_OFFSET				   4*1024
#define	CONFIG_NSIH_SIZE				   4*1024
#define	CONFIG_UBOOT_OFFSET				   8*1024
#define	CONFIG_UBOOT_SIZE				 240*1024

#define CONFIG_KERNEL_OFFSET			 256*1024
#define CONFIG_KERNEL_SIZE				2*1024*1024

#define CONFIG_RAMDISK_OFFSET			0x240000	
#define CONFIG_RAMDISK_SIZE				0x5c0000
#endif	/* CONFIG_CMD_EEPROM */

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

#define CONFIG_EXTRA_ENV_SETTINGS	MTDPARTS_DEFAULT

/*-----------------------------------------------------------------------
 * SDMMC
 */
//#define CONFIG_CMD_MMC
//#define CONFIG_MMC
//#define	CONFIG_GENERIC_MMC
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
#ifdef CFG_BOOT_FROM_USB

#define CONFIG_CMD_USB
#define CONFIG_USB_STORAGE
#define CONFIG_USB_EHCI
#define CONFIG_USB_EHCI_NXP2120

/*-----------------------------------------------------------------------
 * CHECK USB Storage Number
 */
#define CONFIG_CHECK_USB_STORAGE_NUM

/*-----------------------------------------------------------------------
 * USB Lowlevel Init
 */
#define CONFIG_CMD_USB_LOWLEVEL_INIT

/*-----------------------------------------------------------------------
 * BOOTLOGO : USB
 */
#define CONFIG_BOOTLOGO_COMMAND		NULL	

#if defined(CONFIG_BOOTLOGO_COMMAND)
#define CONFIG_BOOTLOGO
#endif

#define CONFIG_BOOTCOMMAND				""
#define CONFIG_BOOTARGS					"root=/dev/sda1 rw rootwait rootfstype=ext4 console=ttyS0,115200n8 androidboot.hardware=nexell androidboot.console=ttyS0 init=/init raid=noautodetect"

#endif // CFG_BOOT_FROM_USB

#ifdef CFG_BOOT_FROM_NAND

/*-----------------------------------------------------------------------
 * NAND FLASH
 */
//#define	CONFIG_SYS_NO_FLASH
//#define	CONFIG_CMD_NAND

#define CONFIG_SYS_64BIT_VSPRINTF		/* needed for nand_util.c (u-boot-09.11)*/

#define CONFIG_SYS_MAX_NAND_DEVICE		1

#define	CONFIG_SYS_NAND_BASE			PHY_BASEADDR_NAND_STATIC	/* Nand conrtoller base	*/
#define CONFIG_MTD_NAND_VERIFY_WRITE								/* set when software ecc mode */
#define CONFIG_SYS_NAND_HW_ECC
//#define CONFIG_CMD_NAND_MLC

#ifdef  CONFIG_CMD_NAND_MLC
#define	CONFIG_SYS_NAND_HW_ECC
#endif

/*-----------------------------------------------------------------------
 * NAND COMMAND
 */
#ifdef CONFIG_CMD_NAND
// for other nand write function
//#define	CFG_NAND_WRITE_YAFFS2
#define	CFG_NAND_WRITE_BAD
#define	NAND_WRITE_BAD					(0x01)
#define	NAND_WRITE_YAFFS2				(0x02)
#endif

/*-----------------------------------------------------------------------
 * BOOTLOGO : NAND
 */
#define CONFIG_BOOTLOGO_COMMAND		NULL	
#if defined(CONFIG_BOOTLOGO_COMMAND)
#define CONFIG_BOOTLOGO
#endif

#endif // CFG_BOOT_FROM_NAND

#define CONFIG_BOOTCOMMAND				""
#define CONFIG_BOOTARGS					"root=/dev/ram0 rootfstype=ext2 ramdisk_size=2048 initrd=0x88000000,2M console=ttyS0,115200n8 androidboot.hardware=nexell androidboot.console=ttyS0 init=/init printk.time=1"

#if !defined(CONFIG_CMD_NAND)

#define	CONFIG_SYS_NO_FLASH
#undef  CONFIG_CMD_IMLS					/* list all images found in flash	*/

#endif // CONFIG_CMD_NAND

/*-----------------------------------------------------------------------
 * CHECK BOOT : check upgrade boot vs normal boot
 */
//#define CONFIG_CHECKBOOT

/*-----------------------------------------------------------------------
 * CHECK POWER KEY
 */
//#define COFNIG_CHECK_POWERKEY

/*-----------------------------------------------------------------------
 * FAT
 */
#if defined(CONFIG_MMC) || defined(CONFIG_CMD_USB)
#define	CONFIG_DOS_PARTITION
#define	CONFIG_CMD_FAT
#endif

/*-----------------------------------------------------------------------
 * Preboot before Kernel boot
 */
//#define CONFOG_BOARD_PREBOOT_OS

/*-----------------------------------------------------------------------
 * Debug message
 */
#define	CONFIG_DISPLAY_CPUINFO			/* print_cpuinfo */
//#define CONFIG_UART_PRE_DEBUG			/* for previous boot message, before board_init */
//#define DEBUG							/* u-boot debug macro, nand, ethernet,... */
//#define CONFIG_PROTO_FUNC_DEBUG		/* nexell prototype debug mode */

#endif /* __CONFIG_H */

