/*
 * (C) Copyright 2010
 * jung hyun kim, Nexell Co, <jhkim@nexell.co.kr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <common.h>
#include <command.h>
#include <i2c.h>

#include "nxe1000_power.h"

/*
 * Debug
 */
#if (0)
#define DBGOUT(msg...)		{ printf("pmic: " msg); }
#define	NXE100_REG_DUMP
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

static u_char nxe1000_cache_reg[48];

static int nxe1000_i2c_read(u8 reg, u8 *value, struct nxe1000_power *power)
{
	uchar chip = power->i2c_addr;
	u32   addr = (u32)reg & 0xFF;
	int   alen = 1;

	return i2c_read(chip, addr, alen, value, 1);
}

static int nxe1000_i2c_write(u8 reg, u8 value, struct nxe1000_power *power)
{
	uchar chip = power->i2c_addr;
	u32   addr = (u32)reg & 0xFF;
	int   alen = 1;

	return i2c_write(chip, addr, alen, &value, 1);
}

#if defined(NXE100_REG_DUMP)
static void nxe1000_register_dump(struct nxe1000_power *power)
{
	u_char *cache = nxe1000_cache_reg;
	printf("+++++++++++++++++++++++++\n");
/*
	nxe1000_i2c_read(NXE1000_REG_PCCNT   , &cache[NXE1000_REG_PCCNT]   , power);
	nxe1000_i2c_read(NXE1000_REG_PCCNT	 , &cache[NXE1000_REG_PCST]    , power);
	nxe1000_i2c_read(NXE1000_REG_VDCTRL  , &cache[NXE1000_REG_VDCTRL]  , power);
	nxe1000_i2c_read(NXE1000_REG_LDOON   , &cache[NXE1000_REG_LDOON]   , power);
	nxe1000_i2c_read(NXE1000_REG_LDO2DAC , &cache[NXE1000_REG_LDO2DAC] , power);
	nxe1000_i2c_read(NXE1000_REG_LDO3DAC , &cache[NXE1000_REG_LDO3DAC] , power);
	nxe1000_i2c_read(NXE1000_REG_LDO4DAC , &cache[NXE1000_REG_LDO4DAC] , power);
	nxe1000_i2c_read(NXE1000_REG_LDO5DAC , &cache[NXE1000_REG_LDO5DAC] , power);
	nxe1000_i2c_read(NXE1000_REG_LDO6DAC , &cache[NXE1000_REG_LDO6DAC] , power);
	nxe1000_i2c_read(NXE1000_REG_LDO7DAC , &cache[NXE1000_REG_LDO7DAC] , power);
	nxe1000_i2c_read(NXE1000_REG_LDO8DAC , &cache[NXE1000_REG_LDO8DAC] , power);
	nxe1000_i2c_read(NXE1000_REG_DDCTL1  , &cache[NXE1000_REG_DDCTL1]  , power);
	nxe1000_i2c_read(NXE1000_REG_DDCTL2  , &cache[NXE1000_REG_DDCTL2]  , power);
	nxe1000_i2c_read(NXE1000_REG_RAMP1CTL, &cache[NXE1000_REG_RAMP1CTL], power);
	nxe1000_i2c_read(NXE1000_REG_RAMP2CTL, &cache[NXE1000_REG_RAMP2CTL], power);
	nxe1000_i2c_read(NXE1000_REG_DD1DAC  , &cache[NXE1000_REG_DD1DAC]  , power);
	nxe1000_i2c_read(NXE1000_REG_DD2DAC  , &cache[NXE1000_REG_DD2DAC]  , power);
	nxe1000_i2c_read(NXE1000_REG_CHGSTART, &cache[NXE1000_REG_CHGSTART], power);
	nxe1000_i2c_read(NXE1000_REG_FET1CNT , &cache[NXE1000_REG_FET1CNT] , power);
	nxe1000_i2c_read(NXE1000_REG_FET2CNT , &cache[NXE1000_REG_FET2CNT] , power);
	nxe1000_i2c_read(NXE1000_REG_TSET    , &cache[NXE1000_REG_TSET]	   , power);
	nxe1000_i2c_read(NXE1000_REG_CMPSET  , &cache[NXE1000_REG_CMPSET]  , power);
	nxe1000_i2c_read(NXE1000_REG_SUSPEND , &cache[NXE1000_REG_SUSPEND] , power);
	nxe1000_i2c_read(NXE1000_REG_SUSPEND , &cache[NXE1000_REG_CHGSTATE], power);
	nxe1000_i2c_read(NXE1000_REG_CHGEN1  , &cache[NXE1000_REG_CHGEN1]  , power);
	nxe1000_i2c_read(NXE1000_REG_CHGIR1  , &cache[NXE1000_REG_CHGIR1]  , power);
	nxe1000_i2c_read(NXE1000_REG_CHGEN2  , &cache[NXE1000_REG_CHGMON1] , power);
	nxe1000_i2c_read(NXE1000_REG_CHGEN2  , &cache[NXE1000_REG_CHGEN2]  , power);
	nxe1000_i2c_read(NXE1000_REG_CHGIR2  , &cache[NXE1000_REG_CHGIR2]  , power);
*/
	printf("[0x%02x]PCCNT     = 0x%02x\n", NXE1000_REG_PCCNT, cache[NXE1000_REG_PCCNT]);
	printf("[0x%02x]PCST      = 0x%02x\n", NXE1000_REG_PCST, cache[NXE1000_REG_PCST]);	/* Read only */
	printf("[0x%02x]VDCTRL    = 0x%02x\n", NXE1000_REG_VDCTRL, cache[NXE1000_REG_VDCTRL]);
	printf("[0x%02x]LDOON     = 0x%02x\n", NXE1000_REG_LDOON, cache[NXE1000_REG_LDOON]);
	printf("[0x%02x]LDO2DAC   = 0x%02x\n", NXE1000_REG_LDO2DAC, cache[NXE1000_REG_LDO2DAC]);
	printf("[0x%02x]LDO3DAC   = 0x%02x\n", NXE1000_REG_LDO3DAC, cache[NXE1000_REG_LDO3DAC]);
	printf("[0x%02x]LDO4DAC   = 0x%02x\n", NXE1000_REG_LDO4DAC, cache[NXE1000_REG_LDO4DAC]);
	printf("[0x%02x]LDO5DAC   = 0x%02x\n", NXE1000_REG_LDO5DAC, cache[NXE1000_REG_LDO5DAC]);
	printf("[0x%02x]LDO6DAC   = 0x%02x\n", NXE1000_REG_LDO6DAC, cache[NXE1000_REG_LDO6DAC]);
	printf("[0x%02x]LDO7DAC   = 0x%02x\n", NXE1000_REG_LDO7DAC, cache[NXE1000_REG_LDO7DAC]);
	printf("[0x%02x]LDO8DAC   = 0x%02x\n", NXE1000_REG_LDO8DAC, cache[NXE1000_REG_LDO8DAC]);
	printf("[0x%02x]DDCTL1    = 0x%02x\n", NXE1000_REG_DDCTL1, cache[NXE1000_REG_DDCTL1]);
	printf("[0x%02x]DDCTL2    = 0x%02x\n", NXE1000_REG_DDCTL2, cache[NXE1000_REG_DDCTL2]);
	printf("[0x%02x]RAMP1CTL  = 0x%02x\n", NXE1000_REG_RAMP1CTL, cache[NXE1000_REG_RAMP1CTL]);
	printf("[0x%02x]RAMP2CTL  = 0x%02x\n", NXE1000_REG_RAMP2CTL, cache[NXE1000_REG_RAMP2CTL]);
	printf("[0x%02x]DD1DAC    = 0x%02x\n", NXE1000_REG_DD1DAC, cache[NXE1000_REG_DD1DAC]);
	printf("[0x%02x]DD2DAC    = 0x%02x\n", NXE1000_REG_DD2DAC, cache[NXE1000_REG_DD2DAC]);
	printf("[0x%02x]CHGSTART  = 0x%02x\n", NXE1000_REG_CHGSTART, cache[NXE1000_REG_CHGSTART]);
	printf("[0x%02x]FET1CNT   = 0x%02x\n", NXE1000_REG_FET1CNT, cache[NXE1000_REG_FET1CNT]);
	printf("[0x%02x]FET2CNT   = 0x%02x\n", NXE1000_REG_FET2CNT, cache[NXE1000_REG_FET2CNT]);
	printf("[0x%02x]TSET      = 0x%02x\n", NXE1000_REG_TSET, cache[NXE1000_REG_TSET]);
	printf("[0x%02x]CMPSET    = 0x%02x\n", NXE1000_REG_CMPSET, cache[NXE1000_REG_CMPSET]);
	printf("[0x%02x]SUSPEND   = 0x%02x\n", NXE1000_REG_SUSPEND, cache[NXE1000_REG_SUSPEND]);
	printf("[0x%02x]CHGSTATE  = 0x%02x\n", NXE1000_REG_CHGSTATE, cache[NXE1000_REG_CHGSTATE]);	/* Read only */
	printf("[0x%02x]CHGEN1    = 0x%02x\n", NXE1000_REG_CHGEN1, cache[NXE1000_REG_CHGEN1]);
	printf("[0x%02x]CHGIR1    = 0x%02x\n", NXE1000_REG_CHGIR1, cache[NXE1000_REG_CHGIR1]);
	printf("[0x%02x]CHGMON1   = 0x%02x\n", NXE1000_REG_CHGMON1, cache[NXE1000_REG_CHGMON1]);	/* Read only */
	printf("[0x%02x]CHGEN2    = 0x%02x\n", NXE1000_REG_CHGEN2, cache[NXE1000_REG_CHGEN2]);
	printf("[0x%02x]CHGIR2    = 0x%02x\n", NXE1000_REG_CHGIR2, cache[NXE1000_REG_CHGIR2]);
	printf("-------------------------\n");
}
#else
#define	nxe1000_register_dump(d)	do { } while(0);
#endif

static int nxe1000_param_setup(struct nxe1000_power *power)
{
	struct nxe1000_vdc_policy  *vdc  = &power->policy.vdc;
	struct nxe1000_ldo_policy  *ldo  = &power->policy.ldo;
	struct nxe1000_dcdc_policy *ddc  = &power->policy.dcdc;
	struct nxe1000_batt_policy *batt = &power->policy.batt;
	struct nxe1000_intc_policy *intc = &power->policy.intc;

	u_char *cache = nxe1000_cache_reg;

	DBGOUT("%s\n", __func__);

	if (! power->support_policy) {
		nxe1000_i2c_read(NXE1000_REG_PCCNT	 , &cache[NXE1000_REG_PCCNT]   , power);
		nxe1000_i2c_read(NXE1000_REG_VDCTRL  , &cache[NXE1000_REG_VDCTRL]  , power);
		nxe1000_i2c_read(NXE1000_REG_LDOON   , &cache[NXE1000_REG_LDOON]   , power);
		nxe1000_i2c_read(NXE1000_REG_LDO2DAC , &cache[NXE1000_REG_LDO2DAC] , power);
		nxe1000_i2c_read(NXE1000_REG_LDO3DAC , &cache[NXE1000_REG_LDO3DAC] , power);
		nxe1000_i2c_read(NXE1000_REG_LDO4DAC , &cache[NXE1000_REG_LDO4DAC] , power);
		nxe1000_i2c_read(NXE1000_REG_LDO5DAC , &cache[NXE1000_REG_LDO5DAC] , power);
		nxe1000_i2c_read(NXE1000_REG_LDO6DAC , &cache[NXE1000_REG_LDO6DAC] , power);
		nxe1000_i2c_read(NXE1000_REG_LDO7DAC , &cache[NXE1000_REG_LDO7DAC] , power);
		nxe1000_i2c_read(NXE1000_REG_LDO8DAC , &cache[NXE1000_REG_LDO8DAC] , power);
		nxe1000_i2c_read(NXE1000_REG_DDCTL1  , &cache[NXE1000_REG_DDCTL1]  , power);
		nxe1000_i2c_read(NXE1000_REG_DDCTL2  , &cache[NXE1000_REG_DDCTL2]  , power);
		nxe1000_i2c_read(NXE1000_REG_RAMP1CTL, &cache[NXE1000_REG_RAMP1CTL], power);
		nxe1000_i2c_read(NXE1000_REG_RAMP2CTL, &cache[NXE1000_REG_RAMP2CTL], power);
		nxe1000_i2c_read(NXE1000_REG_DD1DAC  , &cache[NXE1000_REG_DD1DAC]  , power);
		nxe1000_i2c_read(NXE1000_REG_DD2DAC  , &cache[NXE1000_REG_DD2DAC]  , power);
		nxe1000_i2c_read(NXE1000_REG_CHGSTART, &cache[NXE1000_REG_CHGSTART], power);
		nxe1000_i2c_read(NXE1000_REG_FET1CNT , &cache[NXE1000_REG_FET1CNT] , power);
		nxe1000_i2c_read(NXE1000_REG_FET2CNT , &cache[NXE1000_REG_FET2CNT] , power);
		nxe1000_i2c_read(NXE1000_REG_TSET    , &cache[NXE1000_REG_TSET]	   , power);
		nxe1000_i2c_read(NXE1000_REG_CMPSET  , &cache[NXE1000_REG_CMPSET]  , power);
		nxe1000_i2c_read(NXE1000_REG_SUSPEND , &cache[NXE1000_REG_SUSPEND] , power);
		nxe1000_i2c_read(NXE1000_REG_CHGEN1  , &cache[NXE1000_REG_CHGEN1]  , power);
		nxe1000_i2c_read(NXE1000_REG_CHGIR1  , &cache[NXE1000_REG_CHGIR1]  , power);
		nxe1000_i2c_read(NXE1000_REG_CHGEN2  , &cache[NXE1000_REG_CHGEN2]  , power);
		nxe1000_i2c_read(NXE1000_REG_CHGIR2  , &cache[NXE1000_REG_CHGIR2]  , power);
	} else {
		cache[NXE1000_REG_PCCNT]	 = 0x0;	/* default stand-by operation is invalid */

    	cache[NXE1000_REG_VDCTRL]	 = (((vdc->reset_release_vol&0x7)<<POS_VDCTRL_VD2SEL) |
    								((vdc->battery_release_vol&0x3)<<POS_VDCTRL_VD1SEL) );

    	cache[NXE1000_REG_LDOON]	 = (((ldo->ldo_2_out_enb & 0x1)<<POS_LDOON_LDO2ON) |
    							    ((ldo->ldo_3_out_enb & 0x1)<<POS_LDOON_LDO3ON) |
    							    ((ldo->ldo_4_out_enb & 0x1)<<POS_LDOON_LDO4ON) |
    							   	((ldo->ldo_5_out_enb & 0x1)<<POS_LDOON_LDO5ON) |
    							   	((ldo->ldo_6_out_enb & 0x1)<<POS_LDOON_LDO6ON) |
    							   	((ldo->ldo_7_out_enb & 0x1)<<POS_LDOON_LDO7ON) );

    	cache[NXE1000_REG_LDO2DAC] = ldo->ldo_2_out_vol & 0x07;
    	cache[NXE1000_REG_LDO3DAC] = ldo->ldo_3_out_vol & 0x07;
    	cache[NXE1000_REG_LDO4DAC] = ldo->ldo_4_out_vol & 0x07;
    	cache[NXE1000_REG_LDO5DAC] = ldo->ldo_5_out_vol & 0x07;
    	cache[NXE1000_REG_LDO6DAC] = ldo->ldo_6_out_vol & 0x07;
    	cache[NXE1000_REG_LDO7DAC] = ldo->ldo_7_out_vol & 0x07;
    	cache[NXE1000_REG_LDO8DAC] = ldo->ldo_8_out_vol & 0x07;

    	cache[NXE1000_REG_DDCTL1]  = (((ddc->ddc1_out_reg_on & 0x1)<<POS_DDCTL1_DD1ON)	|
    								  ((ddc->ddc2_out_reg_on & 0x1)<<POS_DDCTL1_DD2ON)	|
									  ((ddc->ddc3_out_reg_on & 0x1)<<POS_DDCTL1_DD3ON) );

		/* default DDnDIS is Enable */
    	cache[NXE1000_REG_DDCTL2] = ((1<<POS_DDCTL2_DD1DIS)	|
    								(1<<POS_DDCTL2_DD2DIS) );

    	cache[NXE1000_REG_RAMP1CTL]= (((!ddc->ddc1_out_ext_ctrl& 0x1)<<POS_RAMP1CTL_DD1ENCTL )	|
    								  ((ddc->ddc1_mode         & 0x3)<<POS_RAMP1CTL_DD1MODE  ) 	|
    								  ((ddc->ddc1_ramp_slop    & 0x3)<<POS_RAMP1CTL_RAMP1SLOP) );

    	cache[NXE1000_REG_RAMP2CTL]= (((!ddc->ddc2_out_ext_ctrl& 0x1)<<POS_RAMP2CTL_DD2ENCTL )	|
    								  ((ddc->ddc2_mode         & 0x3)<<POS_RAMP2CTL_DD2MODE  ) 	|
    								  ((ddc->ddc2_ramp_slop    & 0x3)<<POS_RAMP2CTL_RAMP2SLOP) );

    	cache[NXE1000_REG_DD1DAC]  = ((ddc->ddc1_out_vol & 0x7f) << POS_DD1DAC_DD1DAC);
    	cache[NXE1000_REG_DD2DAC]  = ((ddc->ddc2_out_vol & 0x7f) << POS_DD2DAC_DD2DAC);
   		cache[NXE1000_REG_FET1CNT] = (vdc->sys_limit_current & 0xf);
   		cache[NXE1000_REG_CHGSTART]= (batt->batt_charge_enable & 0x1);

		if (batt->batt_charge_enable) {
    		cache[NXE1000_REG_FET2CNT] = (((batt->charge_current & 0xf) <<POS_FET2CNT_ICHGSET) |
    									  ((batt->charge_voltage & 0x3) <<POS_FET2CNT_CVSET) );

    		cache[NXE1000_REG_TSET]	 = (((batt->charge_time_up & 0x3)<<POS_TSET_RTIMSET) |
    									((batt->charge_temperature_threshold & 0x3)<<POS_TSET_TEMPSET) );

	    	cache[NXE1000_REG_CMPSET]  = (batt->charge_complete_current & 0x7);
    		cache[NXE1000_REG_SUSPEND] = (((batt->charge_ready_current & 0x1)<<POS_SUSPEND_CRCC2) |
    									  ((NXE1000_DEF_CHARGE_SUSPENDB & 0x1)<<POS_SUSPEND_SUSPENDB) );	// default NOT suspend
		} else {
			cache[NXE1000_REG_FET2CNT] = NXE1000_DEF_FET2CNT;
			cache[NXE1000_REG_TSET]	   = NXE1000_DEF_TSET;
			cache[NXE1000_REG_CMPSET]  = NXE1000_DEF_CMPSET;
			cache[NXE1000_REG_SUSPEND] = NXE1000_DEF_SUSPEND;
		}

    	cache[NXE1000_REG_CHGEN1] = (((intc->enb_adapter_in_out   & 0x1)<<POS_CHGEN1_EN_ADPDET) |
    								((intc->enb_die_temperature  & 0x1)<<POS_CHGEN1_EN_DIEOT) |
    								((intc->enb_batt_temperature & 0x1)<<POS_CHGEN1_EN_VBTERR) |
    								((intc->enb_no_batt_dect     & 0x1)<<POS_CHGEN1_EN_NOBATT) |
    								((intc->enb_adapter_over_vol & 0x1)<<POS_CHGEN1_EN_VCOV) |
    								((intc->enb_batt_over_vol    & 0x1)<<POS_CHGEN1_EN_VBOV) );

    	cache[NXE1000_REG_CHGEN2] = (((intc->enb_charge_ready    & 0x1)<<POS_CHGEN2_EN_STCR)   |
    								((intc->enb_charge_state    & 0x1)<<POS_CHGEN2_EN_STRC)   |
    								((intc->enb_charge_complete & 0x1)<<POS_CHGEN2_EN_CHGCMP) |
    								((intc->enb_charge_time_up  & 0x1)<<POS_CHGEN2_EN_TIMEOUT) );
	}

	/* Interrupt requeset clear */
   	cache[NXE1000_REG_CHGIR1]	 = 0;
   	cache[NXE1000_REG_CHGIR2]	 = 0;

	return 0;
}

int nxe1000_device_setup(struct nxe1000_power *power)
{
	u_char *cache = nxe1000_cache_reg;
	int	  bus = power->i2c_bus;

	DBGOUT("%s\n", __func__);
	i2c_init(CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
	i2c_set_bus_num(bus);

	nxe1000_param_setup(power);

	nxe1000_i2c_write(NXE1000_REG_CHGIR1  , cache[NXE1000_REG_CHGIR1] , power);	/* clear irq */
	nxe1000_i2c_write(NXE1000_REG_CHGIR2  , cache[NXE1000_REG_CHGIR2] , power);	/* clear irq */

	nxe1000_i2c_write(NXE1000_REG_CHGEN1  , cache[NXE1000_REG_CHGEN1] , power);
	nxe1000_i2c_write(NXE1000_REG_CHGEN2  , cache[NXE1000_REG_CHGEN2] , power);

	nxe1000_i2c_write(NXE1000_REG_PCCNT	  , cache[NXE1000_REG_PCCNT]   , power);
	nxe1000_i2c_write(NXE1000_REG_VDCTRL  , cache[NXE1000_REG_VDCTRL]  , power);

	nxe1000_i2c_write(NXE1000_REG_LDO2DAC , cache[NXE1000_REG_LDO2DAC] , power);
	nxe1000_i2c_write(NXE1000_REG_LDO3DAC , cache[NXE1000_REG_LDO3DAC] , power);
	nxe1000_i2c_write(NXE1000_REG_LDO4DAC , cache[NXE1000_REG_LDO4DAC] , power);
	nxe1000_i2c_write(NXE1000_REG_LDO5DAC , cache[NXE1000_REG_LDO5DAC] , power);
	nxe1000_i2c_write(NXE1000_REG_LDO6DAC , cache[NXE1000_REG_LDO6DAC] , power);
	nxe1000_i2c_write(NXE1000_REG_LDO7DAC , cache[NXE1000_REG_LDO7DAC] , power);
	nxe1000_i2c_write(NXE1000_REG_LDO8DAC , cache[NXE1000_REG_LDO8DAC] , power);
	nxe1000_i2c_write(NXE1000_REG_LDOON   , cache[NXE1000_REG_LDOON]   , power);

	nxe1000_i2c_write(NXE1000_REG_DDCTL1  , cache[NXE1000_REG_DDCTL1]  , power);
	nxe1000_i2c_write(NXE1000_REG_DDCTL2  , cache[NXE1000_REG_DDCTL2]  , power);
	nxe1000_i2c_write(NXE1000_REG_RAMP1CTL, cache[NXE1000_REG_RAMP1CTL], power);
	nxe1000_i2c_write(NXE1000_REG_RAMP2CTL, cache[NXE1000_REG_RAMP2CTL], power);
	nxe1000_i2c_write(NXE1000_REG_DD1DAC  , cache[NXE1000_REG_DD1DAC]  , power);
	nxe1000_i2c_write(NXE1000_REG_DD2DAC  , cache[NXE1000_REG_DD2DAC]  , power);

	nxe1000_i2c_write(NXE1000_REG_CHGSTART, cache[NXE1000_REG_CHGSTART], power);
	nxe1000_i2c_write(NXE1000_REG_FET1CNT , cache[NXE1000_REG_FET1CNT] , power);
	nxe1000_i2c_write(NXE1000_REG_FET2CNT , cache[NXE1000_REG_FET2CNT] , power);
	nxe1000_i2c_write(NXE1000_REG_TSET    , cache[NXE1000_REG_TSET]	   , power);
	nxe1000_i2c_write(NXE1000_REG_CMPSET  , cache[NXE1000_REG_CMPSET]  , power);
	nxe1000_i2c_write(NXE1000_REG_SUSPEND , cache[NXE1000_REG_SUSPEND] , power);

	nxe1000_register_dump(power);
	return 0;
}


