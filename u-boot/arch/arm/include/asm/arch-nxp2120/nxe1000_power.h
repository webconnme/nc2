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

#ifndef __NXE1000_POWER_H_
#define __NXE1000_POWER_H_

/*
 * register offset
 */

/* Power contrl */
#define	NXE1000_REG_PCCNT			    0x00
#define	NXE1000_REG_PCST                0x01	/* Read only */
/* Dectetiion */
#define	NXE1000_REG_VDCTRL              0x02
/* LDO */
#define	NXE1000_REG_LDOON               0x03
#define	NXE1000_REG_LDO2DAC             0x04
#define	NXE1000_REG_LDO3DAC             0x05
#define	NXE1000_REG_LDO4DAC             0x06
#define	NXE1000_REG_LDO5DAC             0x07
#define	NXE1000_REG_LDO6DAC             0x08
#define	NXE1000_REG_LDO7DAC             0x09
#define	NXE1000_REG_LDO8DAC             0x0A
/* DCDC */
#define	NXE1000_REG_DDCTL1              0x10
#define	NXE1000_REG_DDCTL2              0x11
#define	NXE1000_REG_RAMP1CTL            0x12
#define	NXE1000_REG_RAMP2CTL            0x13
#define	NXE1000_REG_DD1DAC              0x14
#define	NXE1000_REG_DD2DAC              0x15
/* Charger */
#define	NXE1000_REG_CHGSTART            0x20
#define	NXE1000_REG_FET1CNT             0x21
#define	NXE1000_REG_FET2CNT             0x22
#define	NXE1000_REG_TSET                0x23
#define	NXE1000_REG_CMPSET              0x24
#define	NXE1000_REG_SUSPEND             0x25
#define	NXE1000_REG_CHGSTATE            0x26	/* Read only */
/* Interrupt */
#define	NXE1000_REG_CHGEN1              0x28
#define	NXE1000_REG_CHGIR1				0x29
#define	NXE1000_REG_CHGMON1             0x2A	/* Read only */
#define	NXE1000_REG_CHGEN2              0x2C
#define	NXE1000_REG_CHGIR2				0x2D

/*
 * register bit position
 */

/* VDCTRL */
#define	POS_VDCTRL_VD1SEL			(0)
#define	POS_VDCTRL_VD2SEL			(4)

/* LDOON */
#define	POS_LDOON_LDO2ON			(1)
#define	POS_LDOON_LDO3ON			(2)
#define	POS_LDOON_LDO4ON			(3)
#define	POS_LDOON_LDO5ON			(4)
#define	POS_LDOON_LDO6ON			(5)
#define	POS_LDOON_LDO7ON			(6)

/* DDCTL1 */
#define	POS_DDCTL1_DD1ON			(0)
#define	POS_DDCTL1_DD2ON			(1)
#define	POS_DDCTL1_DD3ON			(2)

/* DDCTL2 */
#define	POS_DDCTL2_DD1DIS			(0)
#define	POS_DDCTL2_DD2DIS			(1)

/* RAMP1CTL, RAMP2CTL */
#define	POS_RAMP1CTL_DD1ENCTL		(1)
#define	POS_RAMP1CTL_DD1MODE		(2)
#define	POS_RAMP1CTL_RAMP1SLOP		(4)

#define	POS_RAMP2CTL_DD2ENCTL		(1)
#define	POS_RAMP2CTL_DD2MODE		(2)
#define	POS_RAMP2CTL_RAMP2SLOP		(4)

/* DD1DAC, DD2DAC */
#define	POS_DD1DAC_DD1DAC			(0)
#define	POS_DD2DAC_DD2DAC			(0)

/* FET2CNT */
#define	POS_FET2CNT_ICHGSET			(0)
#define	POS_FET2CNT_CVSET			(5)

/* TSET */
#define	POS_TSET_RTIMSET			(0)
#define	POS_TSET_TEMPSET			(4)

/* SUSPEND */
#define	POS_SUSPEND_SUSPENDB		(0)
#define	POS_SUSPEND_CRCC2			(4)

/* CHGEN1 */
#define	POS_CHGEN1_EN_ADPDET		(0)
#define	POS_CHGEN1_EN_DIEOT			(1)
#define	POS_CHGEN1_EN_VBTERR		(2)
#define	POS_CHGEN1_EN_NOBATT		(5)
#define	POS_CHGEN1_EN_VCOV			(6)
#define	POS_CHGEN1_EN_VBOV			(7)

/* CHGIR1 */
#define	POS_CHGIR1_IR_ADPDET		(0)
#define	POS_CHGIR1_IR_DIEOT			(1)
#define	POS_CHGIR1_IR_VBTERR		(2)
#define	POS_CHGIR1_IR_NOBATT		(5)
#define	POS_CHGIR1_IR_VCOV			(6)
#define	POS_CHGIR1_IR_VBOV			(7)

/* CHGMONI */
#define	POS_CHGMONI_MON_ADPDET		(0)
#define	POS_CHGMONI_MON_DIEOT		(1)
#define	POS_CHGMONI_MON_VBTERR		(2)
#define	POS_CHGMONI_MON_NOBATT		(5)
#define	POS_CHGMONI_MON_VCOV		(6)
#define	POS_CHGMONI_MON_VBOV		(7)

/* CHGEN2 */
#define	POS_CHGEN2_EN_STCR			(0)
#define	POS_CHGEN2_EN_STRC			(2)
#define	POS_CHGEN2_EN_CHGCMP		(3)
#define	POS_CHGEN2_EN_TIMEOUT		(4)

/* CHGIR2 */
#define	POS_CHGIR2_IR_STCR			(0)
#define	POS_CHGIR2_IR_STRC			(2)
#define	POS_CHGIR2_IR_CHGCMP		(3)
#define	POS_CHGIR2_IR_TIMEOUT		(4)

#define	POS_MASK_IR2_SHIFT			(8)

/*
 *	Default Value
 */
#define	NXE1000_DEF_VDC_VD1					2 		/* VAL(V) = 0: 3.1, 1: 3.2,  2: 3.3(default), 3: 3.5 */
#define	NXE1000_DEF_VDC_VD2					7		/* VAL(V) = 0: prohibit, 1: 1.53,  2: 2.13, 3: 2.21, 4: 2.38, 5: 2.42, 6: 2.55, 7: 2.81 (default) */
#define NXE1000_DEF_SYS_LIMIT_CURR 			8		/* VAL(mA) = 0:120, 1:240, 2:360, 3:480, 4:600, 5:720(default), 6:840, 7:960, 8:1080, other:1200 */

#define	NXE1000_DEF_LDO2_VOL				3		/* VAL(V) = 0: 0.90, 1: 1.00,  2: 1.10, 3: 1.20, 4: 1.30, 5: prohibit, 6: prohibit, 7: prohibit */
#define	NXE1000_DEF_LDO3_VOL				3		/* VAL(V) = 0: 0.90, 1: 1.00,  2: 1.10, 3: 1.20, 4: 1.30, 5: prohibit, 6: prohibit, 7: prohibit */
#define	NXE1000_DEF_LDO4_VOL				6		/* VAL(V) = 0: 1.80, 1: 2.50,  2: 2.60, 3: 2.80, 4: 2.85, 5: 3.00, 6: 3.30, 7: prohibit */
#define	NXE1000_DEF_LDO5_VOL				0		/* VAL(V) = 0: 1.80, 1: 2.50,  2: 2.60, 3: 2.80, 4: 2.85, 5: 3.00, 6: 3.30, 7: prohibit */
#define	NXE1000_DEF_LDO6_VOL				3		/* VAL(V) = 0: 1.20, 1: 1.80,  2: 2.50, 3: 2.60, 4: 2.80, 5: 2.85, 6: 3.00, 7: 3.30 */
#define	NXE1000_DEF_LDO7_VOL				6		/* VAL(V) = 0: 1.20, 1: 1.80,  2: 2.50, 3: 2.60, 4: 2.80, 5: 2.85, 6: 3.00, 7: 3.30 */
#define	NXE1000_DEF_LDO8_VOL				7		/* VAL(V) = 0: prohibit, 1: 1.80,  2: 2.50, 3: 2.60, 4: 2.80, 5: 2.85, 6: 3.00, 7: 3.30 */


#define	NXE1000_DEF_DDC1_CTRL_EXT			1		/* VAL = 0: use internal register (DDCTL1), 1: use external signal (DCDCEN12)  */
#define	NXE1000_DEF_DDC2_CTRL_EXT			1		/* VAL = 0: use internal register (DDCTL1), 1: use external signal (DCDCEN12)  */

#define	NXE1000_DEF_DDC1_REG_ON				1		/* VAL = 0: Off, 1: On  */
#define	NXE1000_DEF_DDC2_REG_ON				1		/* VAL = 0: Off, 1: On  */
#define	NXE1000_DEF_DDC3_REG_ON				1		/* VAL = 0: Off, 1: On  */

#define	NXE1000_DEF_DDC1_RAMP_SLOP			1		/* VAL(mV/us) = 0:15, 1:30(default), 2:60, 3:prohibit  */
#define	NXE1000_DEF_DDC1_MODE				2		/* VAL = 0:PSM, 1:PWM, 2:AUTO(default), 3:AUTO */
#define	NXE1000_DEF_DDC1_OUT_VOL			0x18	/* VAL(V) = 0x0: 0.9000, ~, 0x18:1.20000(default), ~, 0x30:1.5000, ~: prohibit, NOTE> step 0.0125 V  */

#define	NXE1000_DEF_DDC2_RAMP_SLOP			1		/* VAL(mV/us) = 0:15, 1:30(default), 2:60, 3:prohibit  */
#define	NXE1000_DEF_DDC2_MODE				2		/* VAL = 0:PSM, 1:PWM, 2:AUTO(default), 3:AUTO */
#define	NXE1000_DEF_DDC2_OUT_VOL			0x48	/* VAL(V) = 0x0: 0.9000, ~, 0x18:1.20000, ~, 0x48:1.8000(default), ~: prohibit, NOTE> step 0.0125 V  */

#define	NXE1000_DEF_CHARGE_ON				0
#define NXE1000_DEF_CHARGE_VOLTAGE			0		/* VAL(V)= 0:4.2(default), 1:4.12, 2:4.07, 3:4.07 */
#define NXE1000_DEF_CHARGE_CURRENT			0		/* VAL(mA)= 0:100 (default), 1:200, 2:300, 3:400, 4:500, 5:600, 6:700, 7:800, other:900 */
#define NXE1000_DEF_CHARGE_COMP_CURRENT		0		/* VAL(mA)= 0:25(deefault), 1:50, 2:75, 3:100, 4:125:, 5:150, 6:175, 7:200 */
#define NXE1000_DEF_CHARGE_TIMPUP			3		/* VAL(min)= 0:120(default), 1:180, 2:240, 3:300 */
#define NXE1000_DEF_CHARGE_TEMP_THS			2		/* VAL(C)= 0: detection(105), recovery(85), 1: detection(115), recovery(95),
															   2: detection(125), recovery(105) (default), 3: detection(135), recovery(115) */
#define NXE1000_DEF_CHARGE_READY_CURRENT	0		/* VAL(mA)= 0:0(deefault), 1:10 */
#define NXE1000_DEF_CHARGE_SUSPENDB			1


/*
 * for no battery
 */
#define NXE1000_DEF_FET1CNT					0x05
#define NXE1000_DEF_FET2CNT					0x00
#define NXE1000_DEF_TSET					0x20
#define NXE1000_DEF_CMPSET					0x00
#define NXE1000_DEF_SUSPEND					0x01
/*
 * Policy to control pmic state machine.
 */
struct nxe1000_pmic_policy;
struct nxe1000_power;

/* Voltage Detector */
struct nxe1000_vdc_policy {
	/* VDCTRL: Detection Circuit Control Register(Address 02h) */
	int	battery_release_vol;	/* R:VDCTRL, VAL(V) = 0: 3.1, 1: 3.2,  2: 3.3(default), 3: 3.5 */
	int	reset_release_vol;		/* R:VDCTRL, VAL(V) = 0: prohibit, 1: 1.53,  2: 2.13, 3: 2.21, 4: 2.38, 5: 2.42, 6: 2.55, 7: 2.81 (default) */
	int	sys_limit_current; 		/* R:FET1CNT, NOTE> this value will affect the VCHG and VSYS pin, VAL(mA) = 0:120, 1:240, 2:360, 3:480, 4:600, 5:720(default), 6:840, 7:960, 8:1080, other:1200 */
};

/* Regulator */
struct nxe1000_ldo_policy {
	/* LDOON: LDO Output Control Register(Address 03h) */
	int	ldo_2_out_enb;		/* R:LDOON  , 1= On, 0 = Off */
	int	ldo_2_out_vol;		/* R:LDO2DAC, Pin VOUT2, VAL(V) = 0: 0.90, 1: 1.00,  2: 1.10, 3: 1.20(default), 4: 1.30, 5: prohibit, 6: prohibit, 7: prohibit */
	int	ldo_3_out_enb;		/* R:LDOON  , 1= On, 0 = Off */
	int	ldo_3_out_vol;		/* R:LDO3DAC, Pin VOUT3, VAL(V) = 0: 0.90, 1: 1.00,  2: 1.10, 3: 1.20(default), 4: 1.30, 5: prohibit, 6: prohibit, 7: prohibit */
	int	ldo_4_out_enb;		/* R:LDOON  , 1= On, 0 = Off */
	int	ldo_4_out_vol;		/* R:LDO4DAC, Pin VOUT4, VAL(V) = 0: 1.80, 1: 2.50,  2: 2.60, 3: 2.80, 4: 2.85, 5: 3.00, 6: 3.30(default), 7: prohibit */
	int	ldo_5_out_enb;		/* R:LDOON  , 1= On, 0 = Off */
	int	ldo_5_out_vol;		/* R:LDO5DAC, Pin VOUT4, VAL(V) = 0: 1.80(default), 1: 2.50,  2: 2.60, 3: 2.80, 4: 2.85, 5: 3.00, 6: 3.30, 7: prohibit */
	int	ldo_6_out_enb;		/* R:LDOON  , 1= On, 0 = Off */
	int	ldo_6_out_vol;		/* R:LDO6DAC, Pin VOUT6, VAL(V) = 0: 1.20, 1: 1.80,  2: 2.50, 3: 2.60(default), 4: 2.80, 5: 2.85, 6: 3.00, 7: 3.30 */
	int	ldo_7_out_enb;		/* R:LDOON  , 1= On, 0 = Off */
	int	ldo_7_out_vol;		/* R:LDO7DAC, Pin VOUT7, VAL(V) = 0: 1.20, 1: 1.80,  2: 2.50, 3: 2.60, 4: 2.80, 5: 2.85, 6: 3.00(default), 7: 3.30 */
	int	ldo_8_out_vol;		/* R:LDO8DAC, Pin VOUT8, VAL(V) = 0: prohibit, 1: 1.80,  2: 2.50, 3: 2.60, 4: 2.80, 5: 2.85, 6: 3.00, 7: 3.30(default) */
							/* LDO8 default is Enable */
};

struct nxe1000_dcdc_policy {
	int ddc1_out_ext_ctrl;	/* R:RAMP1CTL, DDC1 out(LX1 pin) control, VAL = 0: use internal register (DDCTL1), 1: use external signal (DCDCEN12) */
	int ddc2_out_ext_ctrl;	/* R:RAMP1CTL, DDC2 out(LX2 pin) control, VAL = 0: use internal register (DDCTL1), 1: use external signal (DCDCEN12) */
	int ddc1_out_reg_on;	/* R:DDCTL1, when ddc1_out_ext_ctrl is 0, set internal ddc1 on/off register (DDCTL1) */
	int ddc2_out_reg_on;	/* R:DDCTL1, when ddc2_out_ext_ctrl is 0, set internal ddc2 on/off register (DDCTL1) */
	int ddc3_out_reg_on;	/* R:DDCTL1, set internal ddc3 on/off register (DDCTL1) */
	int ddc1_ramp_slop;		/* R:RAMP1CTL, RAMP Slope, VAL(mV/us) = 0:15, 1:30(default), 2:60, 3:prohibit  */
	int ddc1_mode;			/* R:RAMP1CTL, Mode, VAL = 0:PSM, 1:PWM, 2:AUTO(default), 3:AUTO */
	int ddc2_ramp_slop;		/* R:RAMP1CTL, RAMP Slope, VAL(mV/us) = 0:15, 1:30(default), 2:60, 3:prohibit  */
	int ddc2_mode;			/* R:RAMP2CTL, Mode, VAL = 0:PSM, 1:PWM, 2:AUTO(default), 3:AUTO */
	int ddc1_out_vol;		/* R:DD1DAC, DCDC1 Ouput Voltage(LX1 pin), VAL(V) = 0x0: 0.9000, ~, 0x18:1.20000(default), ~, 0x30:1.5000, ~: prohibit, NOTE> step 0.0125 V  */
	int ddc2_out_vol;		/* R:DD2DAC, DCDC2 Ouput Voltage(LX2 pin), VAL(V) = 0x0: 0.9000, ~, 0x18:1.20000, ~, 0x48:1.8000(default), ~: prohibit, NOTE> step 0.0125 V  */
};

struct nxe1000_batt_policy {
	int	batt_charge_enable;				/* R:CHGSTART, support battery charging when VCHG is high and battery is connected, VAL = 0: not support, 1: support */
	int	charge_voltage;					/* R:FET2CNT, Battery Charge Voltage (VBAT pin), VAL(V)= 0:4.2(default), 1:4.12, 2:4.07, 3:4.07 */
	int	charge_current;					/* R:FET2CNT, Battery Rapid Charge Current (VBAT pin), VAL(mA)= 0:100, 1:200, 2:300, 3:400, 4:500, 5:600, 6:700, 7:800, other:900 */
	int charge_time_up;					/* R:TSET, VAL(min)= 0:120(default), 1:180, 2:240, 3:300 */
	int charge_temperature_threshold;	/* R:TSET, VAL(C)= 0: detection (105), recovery (85), 1: detection (115), recovery (95), 2: detection (125), recovery (105) (default), 0: detection (135), recovery (115) */
	int charge_complete_current;		/* R:CMPSET, Charge complete current, VAL(mA)= 0:25(deefault), 1:50, 2:75, 3:100, 4:125:, 5:150, 6:175, 7:200 */
	int charge_ready_current;			/* R:SUSPEND, Charge ready current, VAL(mA)= 0:0(deefault), 1:10 */
};

struct nxe1000_intc_policy {
	/* Charge interrupt */
	int enb_adapter_in_out;			/* R:CHGEN1, Adapter insert & remove interrupt enable, VAL= 0:disable, 1:enable */
	int enb_die_temperature;		/* R:CHGEN1, Die abnormal temperature by SW1 or SW2 in charger interrupt enable, VAL= 0:disable, 1:enable */
	int enb_batt_temperature;		/* R:CHGEN1, Battery abnormal temperature interrupt enable, VAL= 0:disable, 1:enable */
	int enb_no_batt_dect;			/* R:CHGEN1, No Battery detect interrupt enable, VAL= 0:disable, 1:enable */
	int enb_adapter_over_vol;		/* R:CHGEN1, Adapter over voltage interrupt enable (VVCHG>6.2V), VAL= 0:disable, 1:enable */
	int enb_batt_over_vol;			/* R:CHGEN1, Battery over voltage interrupt enable (VVBAT > 4.6V), VAL= 0:disable, 1:enable */

	int enb_charge_ready;			/* R:CHGEN2, Shift to Charge-Ready state interrupt enable, VAL= 0:disable, 1:enable */
	int enb_charge_state;			/* R:CHGEN2,Shift to Rapid-Charge state interrupt enable, VAL= 0:disable, 1:enable */
	int enb_charge_complete;		/* R:CHGEN2, Charge complete interrupt enable, VAL= 0:disable, 1:enable */
	int enb_charge_time_up;			/* R:CHGEN2, Timer time out interrupt enable, VAL= 0:disable, 1:enable */
};

struct nxe1000_pmic_policy {
	struct nxe1000_vdc_policy	vdc;
	struct nxe1000_ldo_policy	ldo;
	struct nxe1000_dcdc_policy	dcdc;
	struct nxe1000_batt_policy	batt;
	struct nxe1000_intc_policy	intc;
};

/*
 * platform device data
 */
struct nxe1000_power {
	int							i2c_bus;
	int							i2c_addr;
	int							support_policy;
	struct nxe1000_pmic_policy	policy;
};

extern int nxe1000_device_setup(struct nxe1000_power *power);

#endif
