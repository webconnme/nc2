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

/* nexell soc headers */
#include <mach-types.h>
#include <platform.h>
#include <platfunc.h>

DECLARE_GLOBAL_DATA_PTR;

#ifndef	CONFIG_ARCH_CPU_INIT
#  error unable to access prototype, must be define the macro "CONFIG_ARCH_CPU_INIT"
#endif

/*------------------------------------------------------------------------------
 * u-boot cpu interface
 */
void lowlevel_init(void)
{
	/* Remap peripheral port */
	asm("mcr p15, 0, %0, c15, c2, 4" : : "r" (0xC0000000 | 0x15));
}

int arch_cpu_init (void)
{
	/* for previous boot message, before board_init */
#ifdef CONFIG_UART_PRE_DEBUG
	#define	CONFIG_DEBUG_LL_UART_PORT 	(0)
	#define	REG_GPIOB_ALTFN_BASE	(*(volatile unsigned  long *)(PHY_BASEADDR_GPIO + 0x60))
	#define	REG_GPIOB_ALTFN_NUM		(CONFIG_DEBUG_LL_UART_PORT + 1) * 2
	//REG_GPIOB_ALTFN_BASE = (REG_GPIOB_ALTFN_BASE & ~(3<<(REG_GPIOB_ALTFN_NUM*2))) |
													//(1<<(REG_GPIOB_ALTFN_NUM*2));
	REG_GPIOB_ALTFN_BASE = (REG_GPIOB_ALTFN_BASE & ~(3) | (1));
 	gd->bd->bi_baudrate = 115200;
 	serial_init();
	serial_puts("\nSet uart for previous debug message ...\n");
#endif

#if defined	(CONFIG_PM_WAKEUP) || defined(CONFIG_PM_WAKEUP_ASM)
	cpu_mach_wakeup();
#endif
	cpu_mach_init();

	/* set global data */
	gd->bd->bi_arch_number = MACH_TYPE_NXP2120;
	gd->bd->bi_boot_params = CFG_KERNEL_TEXT_BASE + 0x00000100;

	return 0;
}

int print_cpuinfo(void)
{
	unsigned int reg_sp, reg_pc;

	asm("mov %0, sp":"=r" (reg_sp));
	asm("mov %0, pc":"=r" (reg_pc));

//	printf("U-Boot Text=0x%08x, End=0x%08x \n", (uint)_armboot_start, (uint)_bss_end);
#if defined(CONFIG_SYS_HEAP_LEN)
	#if defined(CONFIG_SYS_HEAP_BASE)
//	printf("U-Boot Heap=0x%08x, End=0x%08x \n",
//		CONFIG_SYS_HEAP_BASE, CONFIG_SYS_HEAP_BASE+CONFIG_SYS_HEAP_LEN);
	#else
//	printf("U-Boot Heap=0x%08x, End=0x%08x \n",
//		(uint)_bss_end,	(uint)_bss_end+CONFIG_SYS_HEAP_LEN);
	#endif
#endif
//	printf("U-Boot PC  =0x%08x, SP =0x%08x \n", reg_pc, reg_sp);

	cpu_stat_info();

#if defined(CONFIG_GENERIC_MMC)
	sdhc_arch_init();
#endif
	return 0;
}

/* u-boot dram interface */
int dram_init(void)
{
	gd->bd->bi_dram[0].start = CFG_MEM_VIR_SYSTEM_BASE;
#if defined(CFG_MEM_PHY_LINEAR_SIZE)
	gd->bd->bi_dram[0].size  = CFG_MEM_PHY_SYSTEM_SIZE + CFG_MEM_PHY_LINEAR_SIZE;
#else
	gd->bd->bi_dram[0].size  = CFG_MEM_PHY_SYSTEM_SIZE;
#endif
	return 0;
}

/*------------------------------------------------------------------------------
 * u-boot cpu arch initialize
 */

/* u-boot miscellaneous arch dependent initialisations */
#if defined(CONFIG_ARCH_MISC_INIT)
int arch_misc_init(void)
{
	return (0);
}
#endif	/* CONFIG_ARCH_MISC_INIT */

/*------------------------------------------------------------------------------
 * u-boot misc initialize
 */

/* u-boot miscellaneous initialisations */
#if defined(CONFIG_MISC_INIT_R)
int misc_init_r(void)
{
	return (0);
}
#endif	/* CONFIG_MISC_INIT_R */

void arch_preboot_os(void)
{
#ifdef CONFOG_BOARD_PREBOOT_OS
extern void board_preboot_os(void);
	board_preboot_os();
#endif
}
