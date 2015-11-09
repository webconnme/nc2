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
#ifndef __CFG_MEM_H__
#define __CFG_MEM_H__

/*------------------------------------------------------------------------------
 * 	Linux dram memory map
 *
 *	|					|
 *	|	Linux Kernel	|
 *	|	Code			|
 *	|					|
 *	|	Linux Kernel	|
 *	|	PTS				|
 *	-------------------------	0x80208000	: Linux PTS and Kernel Code
 *	|	Linux Kernel	|
 *	|	Params			|
 *	-------------------------	0x80200100	: Linux Params
 *	|   suspend mode	|
 *	|	back up data	|
 *	=========================	0x80200000	: Linux Kernel TEXT_BASE
 *  |                   |
 *  |   U-Boot Code     |
 *  |                   |
 *  -------------------------   0x80100000  : U-Boot TEXT_BASE
 *  |                   |            		: U-Boot initialzie heap size
 *  | U-Boot Heap 		|					: CONFIG_SYS_MALLOC_LEN (512K)
 *  | 	(BLD Stack)    	|
 *  |                   |
 *  -------------------------   0x80080000  : U-Boot Stack
 *  | U-Boot Stack      |					: CONFIG_STACKSIZE (256k)
 *  |	(BLD Code)    	|
 *  .........................   0x80010000  : BLD TEXT_BASE
 *  |   (BLD PTS)		|       (64K)
 *  .........................   0x80004000  : BLD page table (PTS)
 *  |   Nand ECC        |       (16K ~ 32K)
 *  |   or vector table |
 *  =========================   0x80000000  : Memory Base
 *
 ------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 * 	 Kernel / Suspend zone
 */
#define	CFG_KERNEL_TEXT_BASE			0x80000000	/* for bootargs */

/*------------------------------------------------------------------------------
 *   System memory map
 */
#define CFG_MEM_VIR_SYSTEM_BASE         0x80000000  /* System, must be at an evne 2MB boundary (head.S) */
#define CFG_MEM_PHY_SYSTEM_BASE         0x80000000  /* System, must be at an evne 2MB boundary (head.S) */
#define CFG_MEM_PHY_SYSTEM_SIZE         0x06000000  /* 96 MB */

/*------------------------------------------------------------------------------
 *   Reserved memory map
 */
#define CFG_MEM_PHY_LINEAR_BASE         0x86000000  /* System / DMA */
#define CFG_MEM_PHY_LINEAR_SIZE         0x01000000  /* 16 MB */

#define CFG_MEM_PHY_BLOCK_BASE          0x87000000  /* For video */
#define CFG_MEM_PHY_BLOCK_SIZE          0x01000000  /* 16 MB */

#endif /* __CFG_MEM_H__ */
