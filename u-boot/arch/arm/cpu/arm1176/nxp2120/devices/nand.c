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
#include <common.h>
#include <nand.h>
#include <asm/io.h>

/* nexell soc headers */
#include <platform.h>

#include "nand_ecc.h"

/* degug print */
#if	(0)
#define DBGOUT(msg...)		{ printk(KERN_INFO "nand: " msg); }
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

#define ERROUT(msg...)		{ 					\
		printf("ERROR: %s, %s line %d: \n",		\
			__FILE__, __FUNCTION__, __LINE__),	\
		printf(msg); }

#define CLEAR_RnB(r)							\
	r = NX_MCUS_GetInterruptPending(0);			\
	if (r) {									\
		NX_MCUS_ClearInterruptPending(0); 		\
		NX_MCUS_GetInterruptPending  (0); 		\
	}

/*----------------------------------------------------------------------------
 * u-boot nand command
 */
#if defined(CONFIG_CMD_NAND)
/* hw control */
static int  nand_dev_ready  (struct mtd_info *mtd);
static void nand_dev_select (struct mtd_info *mtd, int chipnr);
static void nand_dev_ctrl   (struct mtd_info *mtd, int dat, unsigned int ctrl);

/* hw ecc control */
#ifdef CONFIG_SYS_NAND_HW_ECC
extern int nand_hw_ecc_init (struct mtd_info *mtd);
extern int nand_hw_ecc_check(struct mtd_info *mtd);
#endif
int nand_dev_check(struct mtd_info *mtd);

/*------------------------------------------------------------------------------
 * u-boot nand interface
 */
int board_nand_init(struct nand_chip *chip)
{
	int ret = 0;
	DBGOUT("%s\n", __func__);

	NX_MCUS_SetAutoResetEnable(HW_NAND_AUTORESET);

	NX_MCUS_ClearInterruptPending(0);
	NX_MCUS_SetInterruptEnableAll(CFALSE);

	NX_MCUS_SetNFBank(0);
	NX_MCUS_SetNFCSEnable(CFALSE);

	/* insert callbacks */
	chip->IO_ADDR_R 	= (void __iomem *)CONFIG_SYS_NAND_BASE;
	chip->IO_ADDR_W 	= (void __iomem *)CONFIG_SYS_NAND_BASE;
	chip->cmd_ctrl 		= nand_dev_ctrl;
	chip->dev_ready 	= nand_dev_ready;
	chip->select_chip 	= nand_dev_select;
	chip->chip_delay 	= 15;

#if defined(CONFIG_SYS_NAND_HW_ECC)
	ret = nand_hw_ecc_init(&nand_info[0]);
	printf("Hardware ecc, ");
#elif defined (CONFIG_MTD_NAND_ECC_BCH)
	chip->ecc.mode 	 = NAND_ECC_SOFT_BCH;

	/* refer to nand_ecc_xxx.c (nand_hwecc_init) */
	switch (SW_BCH_ECC) {
	case  4: chip->ecc.bytes =    7;
			 chip->ecc.size  =  512;	break;
	case  8: chip->ecc.bytes =   13;
			 chip->ecc.size  =  512;	break;
    case 12: chip->ecc.bytes =   20;
    		 chip->ecc.size  =  512;	break;
	case 16: chip->ecc.bytes =   26;
			 chip->ecc.size  =  512;	break;
	case 24: chip->ecc.bytes =   42;
			 chip->ecc.size  = 1024;	break;
	case 40: chip->ecc.bytes =   70;
			 chip->ecc.size  = 1024;	break;
	default:
		printk("Fail: not supoort bch ecc %d mode !!!\n", SW_BCH_ECC);
		return -1;
	}
	printk(KERN_INFO "NAND ecc: Software BCH \n");
#else
	chip->ecc.mode = NAND_ECC_SOFT;
	printf("Software ecc, ");
#endif
	return ret;
}

/*------------------------------------------------------------------------------
 * u-boot hw control
 */
#if defined (CONFIG_MTD_NAND_ECC_BCH)
static int nand_swbch_layout_check(struct mtd_info *mtd)
{
	struct nand_chip *chip = mtd->priv;
	struct nand_ecclayout *layout = &chip->ecc.layout[0];
	struct nand_oobfree *oobfree  = layout->oobfree;
	int ecctotal = chip->ecc.total;
	int oobsize	 = mtd->oobsize;

	printk("sw bch ecc %d bit, oob %2d, bad '0,1', ecc %d~%d (%d), free %d~%d (%d) ",
		SW_BCH_ECC, oobsize, oobfree->offset+oobfree->length, oobsize-1, ecctotal,
		oobfree->offset, oobfree->length + 1, oobfree->length);
	return 0;
}
#endif

int nand_dev_check(struct mtd_info *mtd)
{
	int ret = 0;
#if defined (CONFIG_SYS_NAND_HW_ECC)
	ret = nand_hw_ecc_check(mtd);
#elif defined (CONFIG_MTD_NAND_ECC_BCH)
	ret = nand_swbch_layout_check(mtd);
#endif
	return ret;
}

static void nand_dev_select(struct mtd_info *mtd, int chipnr)
{
	DBGOUT("%s, chipnr=%d\n", __func__, chipnr);
#if defined(CFG_NAND_OPTIONS)
	struct nand_chip *chip = mtd->priv;
	chip->options |= CFG_NAND_OPTIONS;
#endif

	if (chipnr > 4) {
		ERROUT("not support nand chip index %d\n", chipnr);
		return;
	}

	if (-1 == chipnr) {
		NX_MCUS_SetNFCSEnable(CFALSE);		// nand chip select control disable
	} else {
		NX_MCUS_SetNFBank(chipnr);
		NX_MCUS_SetNFCSEnable(CTRUE);
	}
}

#define MASK_CLE		0x10
#define MASK_ALE		0x18

static void nand_dev_ctrl(struct mtd_info *mtd, int cmd, unsigned int ctrl)
{
	struct nand_chip *chip = mtd->priv;
	void __iomem* addr = chip->IO_ADDR_W;
	int ret = 0;

	if (cmd == NAND_CMD_NONE)
		return;

	if (ctrl & NAND_CLE)
		writeb(cmd, addr + MASK_CLE);
	else if (ctrl & NAND_ALE)
		writeb(cmd, addr + MASK_ALE);

	if (cmd != NAND_CMD_RESET &&
		cmd != NAND_CMD_READSTART)
		CLEAR_RnB(ret);
}

static int nand_dev_ready(struct mtd_info *mtd)
{
	int ret = 0;
	CLEAR_RnB(ret);
//	DBGOUT("[%s, RnB=%d]\n", ret?"READY":"BUSY", NX_MCUS_IsNFReady());
	return ret;
}

#endif	/* CONFIG_CMD_NAND */



