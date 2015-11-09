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
#include <netdev.h>

/*------------------------------------------------------------------------------
 * u-boot eth interface
 */
#include <net.h>
#include <platform.h>

#ifdef CONFIG_CMD_NET

int board_eth_init(bd_t *bis)
{
	int rc = 0;
	int grp = (CFG_PIO_ETH_RESET >> 5);
	int bit = (CFG_PIO_ETH_RESET & 0x1F);

	NX_GPIO_SetOutputValue (grp, bit, CTRUE );	mdelay(10);
	NX_GPIO_SetOutputValue (grp, bit, CFALSE);	mdelay(50);
	NX_GPIO_SetOutputValue (grp, bit, CTRUE );	mdelay(10);

	rc = smc911x_initialize(0, CONFIG_SMC911X_BASE);
	return rc;
}
#endif	/* CONFIG_CMD_NET */

