/*
 * (C) Copyright 2008-2010 Freescale Semiconductor, Inc.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/* #define DEBUG */

#include <common.h>

#include <command.h>
#include <environment.h>
#include <linux/stddef.h>
#include <malloc.h>
#include <mmc.h>

/* references to names in env_common.c */
extern uchar default_environment[];

char *env_name_spec = "MMC";

#ifdef ENV_IS_EMBEDDED
extern uchar environment[];
env_t *env_ptr = (env_t *)(&environment[0]);
#else /* ! ENV_IS_EMBEDDED */
env_t *env_ptr = NULL;
#endif /* ENV_IS_EMBEDDED */

/* local functions */
#if !defined(ENV_IS_EMBEDDED)
static void use_default(void);
#endif

DECLARE_GLOBAL_DATA_PTR;

uchar env_get_char_spec(int index)
{
	return *((uchar *)(gd->env_addr + index));
}

int env_init(void)
{
	/* use default */
	gd->env_addr = (ulong)&default_environment[0];
	gd->env_valid = 1;

	return 0;
}

#ifdef CONFIG_CMD_SAVEENV
int saveenv(void)
{
	struct mmc *mmc = find_mmc_device(CONFIG_SYS_MMC_ENV_DEV);
	mmc_init(mmc);
	printf("Writing to MMC(%d)... ", CONFIG_SYS_MMC_ENV_DEV);
	mmc->block_dev.block_write(CONFIG_SYS_MMC_ENV_DEV, CONFIG_MMC_ENV_BLOCK_OFFSET, CONFIG_MMC_ENV_BLOCK_COUNT, (u_char *)env_ptr);

	puts("done\n");
	return 0;
}
#endif /* CONFIG_CMD_SAVEENV */

void env_relocate_spec(void)
{
#if !defined(ENV_IS_EMBEDDED)

	struct mmc *mmc = find_mmc_device(CONFIG_SYS_MMC_ENV_DEV);
	mmc_init(mmc);
	mmc->block_dev.block_read(CONFIG_SYS_MMC_ENV_DEV, CONFIG_MMC_ENV_BLOCK_OFFSET, CONFIG_MMC_ENV_BLOCK_COUNT, (u_char *)env_ptr);

#endif
}

#if !defined(ENV_IS_EMBEDDED)
static void use_default()
{
	set_default_env();
}
#endif
