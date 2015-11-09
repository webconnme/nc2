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
//#define CONFIG_SYS_HEAP_LEN				(2*1024*1024)	/* for system heap, for UBIFS 2M */

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
#define CONFIG_BOOTDELAY	   			2
#define CONFIG_ZERO_BOOTDELAY_CHECK
#define CONFIG_ETHADDR		   			12:2b:45:4a:59:24:56
#define CONFIG_NETMASK		   			255.255.255.0
#define CONFIG_IPADDR					192.168.1.214
#define CONFIG_SERVERIP					192.168.1.32
#define CONFIG_GATEWAYIP				192.168.1.254
#define CONFIG_BOOTFILE					"uImage"  		/* File to load	*/
#define CONFIG_BOOTCOMMAND				"udown 81000000; udown 82000000; zimage 81000000"

/*-----------------------------------------------------------------------
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_PROMPT				"nxp2121# "     						/* Monitor Command Prompt   */
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

/*-----------------------------------------------------------------------
 * Ethernet configuration
 * depend on CONFIG_CMD_NET
 */
#undef CONFIG_CMD_NET

/*-----------------------------------------------------------------------
 * NAND FLASH
 */
#define	CONFIG_CMD_NAND
//#define CONFIG_ENV_IS_IN_NAND

#if defined(CONFIG_CMD_NAND)

	#define CONFIG_SYS_MAX_NAND_DEVICE		1
	#define	CONFIG_SYS_NAND_BASE			PHY_BASEADDR_NAND_STATIC	/* Nand conrtoller base	*/

	#define CONFIG_MTD_NAND_VERIFY_WRITE								/* set when software ecc mode */
	#define CONFIG_SYS_NAND_HW_ECC

	#if defined(CONFIG_ENV_IS_IN_NAND)
		#define	CONFIG_ENV_OFFSET			512*1024					/* 0x00080000 */
		#define CONFIG_ENV_SIZE           	16*1024						/* 1 block size */
		#define CONFIG_ENV_RANGE			CONFIG_ENV_SIZE * 4 		/* avoid bad block */
	#endif

//	#define CONFIG_MTD_DEBUG
//	#define CONFIG_MTD_DEBUG_VERBOSE		3	/* For nand debug message = 0 ~ 3 */

	/* etc configure macro */
	#undef  CONFIG_CMD_IMLS												/* list all images found in flash	*/
#endif

/*-----------------------------------------------------------------------
 * EEPROM
 */
#define CONFIG_CMD_EEPROM
#define CONFIG_SPI								/* SPI EEPROM, not I2C EEPROM */
#define CONFIG_ENV_IS_IN_EEPROM				/* default: CONFIG_ENV_IS_NOWHERE */

#if defined(CONFIG_CMD_EEPROM)
	#if defined(CONFIG_SPI)
		#define CONFIG_SPI_EEPROM					/* build spi_eeprom.c */

		/*
 	  	 *	EEPROM Environment Organization
 	 	 *	[Note R/W unit 64K]
 	 	 *
 	 	 *   0 ~   4K First Boot
 	 	 *   4 ~   8K NSIH
 	 	 *   8 ~ 248K U-Boot
 	 	 * 248 ~ 256K Environment
 	 	 */
 	 	#define	CONFIG_SPI_EEPROM_WP_GROUP			2
 	 	#define	CONFIG_SPI_EEPROM_WP_PADNUM			17

		#define	CONFIG_1STBOOT_OFFSET			   	0
		#define	CONFIG_1STBOOT_SIZE				   	4*1024
		#define	CONFIG_NSIH_OFFSET				   	4*1024
		#define	CONFIG_NSIH_SIZE				   	4*1024
		#define	CONFIG_UBOOT_OFFSET				   	8*1024
		#define	CONFIG_UBOOT_SIZE				 	240*1024
		#define	CONFIG_CMD_SPI_EEPROM_UPDATE		/* build cmd_spi_eeprom.c */

		#if defined(CONFIG_ENV_IS_IN_EEPROM)
			#define	CONFIG_ENV_OFFSET				248*1024	/* 248 ~ 256K Environment */
			#define CONFIG_ENV_SIZE           		8*1024
			#define CONFIG_ENV_RANGE				CONFIG_ENV_SIZE
			#define CONFIG_SYS_DEF_EEPROM_ADDR		0					/* Need 0, when SPI */
			#define CONFIG_SYS_I2C_FRAM										/* To avoid max length limit when spi write */
			//#define DEBUG_ENV
		#endif
	#endif
#endif

/*-----------------------------------------------------------------------
 * NOR FLASH
 */
#define	CONFIG_SYS_NO_FLASH

/*-----------------------------------------------------------------------
 * BOOTLOGO
 */
//#define CONFIG_BOOTLOGO_COMMAND			"nand read 8ec00000 600000 c0000; bootlogo 8ec00000"
#define CONFIG_BOOTLOGO

/*-----------------------------------------------------------------------
 * Default environment organization
 */
#if !defined(CONFIG_ENV_IS_IN_MMC) && !defined(CONFIG_ENV_IS_IN_NAND) && 	\
	!defined(CONFIG_ENV_IS_IN_FLASH) && !defined(CONFIG_ENV_IS_IN_EEPROM)
	#define CONFIG_ENV_IS_NOWHERE						/* default: CONFIG_ENV_IS_NOWHERE */
	#define CONFIG_ENV_SIZE           		4*1024		/* env size */
	#undef	CONFIG_CMD_IMLS								/* imls - list all images found in flash, default enable so disable */
#endif

/*-----------------------------------------------------------------------
 * Debug message
 */
#define	CONFIG_DISPLAY_CPUINFO			/* print_cpuinfo */
//#define CONFIG_UART_PRE_DEBUG			/* for previous boot message, before board_init */
//#define DEBUG							/* u-boot debug macro, nand, ethernet,... */
//#define CONFIG_PROTO_FUNC_DEBUG		/* nexell prototype debug mode */

#endif /* __CONFIG_H */

