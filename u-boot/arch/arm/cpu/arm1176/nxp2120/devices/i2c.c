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
#include <command.h>
#include <i2c.h>

#include <platform.h>

#define	DBG_I2C		(0)

#if	(DBG_I2C)
#define DBGOUT(msg...)		do { printf("i2c" msg); } while (0)
#else
#define DBGOUT(msg...)		do {} while (0)
#endif

/*---------------------------------------------------------------------------*/
#define	NUMBER_OF_I2C_MODULE	2

#define	I2C_DELAY_HZ			100000
#define	I2C_DELAY_CYCLE			20		/* PER 100 khz, when CPU 700MHZ(ARM1176), No Cache */

struct i2c_param {
	int				bus;
	unsigned int	scl_io;
	unsigned int	sda_io;
	int				no_stop;
	unsigned int	delay;
	unsigned int	speed;
};

static struct i2c_param i2c_par[2] = {
	{ 0, CFG_PIO_I2C0_SCL, CFG_PIO_I2C0_SDA, CONFIG_SOC_I2C_NOSTOP, CONFIG_SYS_I2C_SPEED, },
	{ 1, CFG_PIO_I2C1_SCL, CFG_PIO_I2C1_SDA, CONFIG_SOC_I2C_NOSTOP, CONFIG_SYS_I2C_SPEED, },
};

static struct i2c_param *i2c_cur = NULL;
static int 			     i2c_bus = 0;

/*----------------------------------------------------------------------------
 * I2C control macro
 */
#define SCL_HIGH(_io)		NX_GPIO_SetOutputValue 	(PAD_GET_GRP(_io), PAD_GET_BIT(_io), CTRUE )
#define SCL_LOW(_io)		NX_GPIO_SetOutputValue 	(PAD_GET_GRP(_io), PAD_GET_BIT(_io), CFALSE)
#define SCL_OUTPUT(_io)		NX_GPIO_SetOutputEnable	(PAD_GET_GRP(_io), PAD_GET_BIT(_io), CTRUE )
#define SCL_INPUT(_io)		NX_GPIO_SetOutputEnable	(PAD_GET_GRP(_io), PAD_GET_BIT(_io), CFALSE)
#define SCL_DATA(_io)		NX_GPIO_GetInputValue  	(PAD_GET_GRP(_io), PAD_GET_BIT(_io))
#define SDA_HIGH(_io)		NX_GPIO_SetOutputValue 	(PAD_GET_GRP(_io), PAD_GET_BIT(_io), CTRUE )
#define SDA_LOW(_io)		NX_GPIO_SetOutputValue 	(PAD_GET_GRP(_io), PAD_GET_BIT(_io), CFALSE)
#define SDA_OUTPUT(_io)		NX_GPIO_SetOutputEnable	(PAD_GET_GRP(_io), PAD_GET_BIT(_io), CTRUE )
#define SDA_INPUT(_io)		NX_GPIO_SetOutputEnable	(PAD_GET_GRP(_io), PAD_GET_BIT(_io), CFALSE)
#define SDA_DATA(_io)		NX_GPIO_GetInputValue  	(PAD_GET_GRP(_io), PAD_GET_BIT(_io))

#define I2C_DELAY(_n)		{ volatile u_int x=0; while ((cyc)*_n > x++); }	/* default _d = 0x200 */

#define	SHT		(2)		/* start  hold  time */
#define	EST		(2)		/* Stop   setup time */

#define	DHT		(1)		/* data   hold  time */
#define	DST		(1)		/* data   setup time */
#define	CHT		(1)		/* clock  high  time */

/* 			________		 ___________		______
 *	<SDA>	 		|_______|			|_______|
 *			 <1>|<1>|SHT|DHT|DST|CHT|DHT|DST|EST|<1>
 *			____________		 ___		 __________
 *	<SCL>		 		|_______|	|_______|
 */

/*----------------------------------------------------------------------------*/
static inline void pio_start(struct i2c_param *par)
{
	u_int scl = par->scl_io;
	u_int sda = par->sda_io;
	u_int cyc = par->delay;

	/* SCL/SDA High */
	SDA_HIGH	(sda);
	SDA_OUTPUT	(sda);
	I2C_DELAY	(1);

	SCL_HIGH	(scl);
	SCL_OUTPUT	(scl);
	I2C_DELAY	(1);

	/* START signal */
	SDA_LOW		(sda);	/* Start condition */
	I2C_DELAY	(SHT);	/* Start hold */

	SCL_LOW		(scl);
	I2C_DELAY	(DHT);		/* Data  hold */
}

static inline void pio_stop(struct i2c_param *par)
{
	u_int scl = par->scl_io;
	u_int sda = par->sda_io;
	u_int cyc = par->delay;

	/* STOP signal */
	SDA_LOW		(sda);
	SDA_OUTPUT	(sda);
	I2C_DELAY	(DST);

	SCL_HIGH	(scl);
	I2C_DELAY	(EST);

	SDA_HIGH	(sda);
	I2C_DELAY	(1);

	SCL_INPUT	(scl);
	SDA_INPUT	(sda);
}

static inline int pio_putbyte(struct i2c_param *par, unsigned char data)
{
	u_int scl = par->scl_io;
	u_int sda = par->sda_io;
	u_int cyc = par->delay;
	int i, nack = 0;

	SDA_OUTPUT	(sda);

	for (i=7 ; i >= 0 ; i--) {
		if (data & (1<<i))
			SDA_HIGH(sda);
		else
			SDA_LOW	(sda);

		I2C_DELAY	(DST);

		SCL_HIGH	(scl);
		I2C_DELAY	(CHT);

		SCL_LOW		(scl);
		I2C_DELAY	(DHT);
	}

	SDA_INPUT	(sda);
	I2C_DELAY	(DST);

	SCL_HIGH	(scl);
	I2C_DELAY	(CHT);

	/* Falling Edge */
#if (0)
	for (i = 0; (ACK_WAIT_TIMEOUT * 1000) > i; i++) {
		nack = SDA_DATA	(sda);
		if (! nack) break;
		udelay(1);
	}
#else
	nack = SDA_DATA(sda);
#endif

	SCL_LOW		(scl);
	I2C_DELAY	(DHT);

	SDA_INPUT	(sda);	/* END */

	return (nack ? -1 : 0);
}

static inline u_char pio_getbyte(struct i2c_param *par, bool ack)
{
	u_int  scl = par->scl_io;
	u_int  sda = par->sda_io;
	u_int  cyc = par->delay;
	u_char dat = 0;
	int   i;

	SDA_INPUT	(sda);

	for ( i=7; i >= 0; i-- ) {

		I2C_DELAY	(DST);
		SCL_HIGH	(scl);
		I2C_DELAY	(CHT);

		/* Falling Edge */
		if (SDA_DATA(sda))
			dat = (unsigned char)(dat | (1<<i));
		else
			dat = (unsigned char)(dat | (0<<i));

		SCL_LOW		(scl);
		I2C_DELAY	(DHT);
	}

	SDA_OUTPUT(sda);

	if (ack)
		SDA_LOW	(sda);
	else
		SDA_HIGH(sda);

	I2C_DELAY	(DST);

	SCL_HIGH	(scl);
	I2C_DELAY	(CHT);

	SCL_LOW		(scl);
	I2C_DELAY	(DHT);

	SDA_INPUT	(sda);	/* END */

	return dat;
}

/*----------------------------------------------------------------------------
 * I2C u-boot
 */
#if defined(CONFIG_I2C_MULTI_BUS)
int i2c_set_bus_num (unsigned int bus)
{
	if (bus > NUMBER_OF_I2C_MODULE-1) {
		printf("i2c bus %d is not exist (max bus %d)\n", bus, NUMBER_OF_I2C_MODULE-1);
		return -1;
	}
	DBGOUT("[%d]: set to bus=%d\n", );

	if (NULL == i2c_cur) 
		i2c_init(CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
	
	i2c_cur = &i2c_par[bus];
	i2c_bus = bus;
	return 0;
}

unsigned int i2c_get_bus_num(void)
{
	DBGOUT("[%d]: get bus=%d\n", i2c_bus, i2c_bus);
	return i2c_bus;
}
#endif

int i2c_set_bus_speed(unsigned int speed)
{
	if (NULL == i2c_cur) 
		i2c_init(CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);

	i2c_cur = &i2c_par[i2c_bus];
	i2c_cur->speed = speed;
	i2c_cur->delay = (speed > I2C_DELAY_HZ) ? I2C_DELAY_CYCLE/(speed/I2C_DELAY_HZ) :
						I2C_DELAY_CYCLE*(I2C_DELAY_HZ/speed);

	DBGOUT("i2c speed=%d, delay=%d\n", i2c_cur->speed, i2c_cur->delay);
	return 0;
}

unsigned int i2c_get_bus_speed(void)
{
	if (NULL == i2c_cur)
		return 0;
	return i2c_cur->speed;
}

int i2c_write(u8 chip, u32 addr, int alen, u8 *buffer, int len)
{
	struct i2c_param *i2c = i2c_cur;
	int no_stop = i2c->no_stop;

	u8  addr_bytes[3]; /* lowest...highest byte of data address */
	u8  data;
	int ret  = -1;

	chip &= 0xFE;

#if (DBG_I2C)
{
	int i = 0;
	printf("i2c[%d]: W chip=0x%2x, addr=0x%x, alen=%d, wlen=%d ", i2c_bus, chip, addr, alen, len);
	for (; len > i; i++)
		printf("[%d]:0x%2x ", i, buffer[i]);
	printf("\n");
}
#endif

	if (NULL == i2c) {
		i2c_init(CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
		i2c = i2c_cur = &i2c_par[i2c_bus];
	}

	/*
	 * send memory address bytes;
	 * alen defines how much bytes we have to send.
	 */
	addr_bytes[0] = (u8)((addr >>  0) & 0x000000FF);
	addr_bytes[1] = (u8)((addr >>  8) & 0x000000FF);
	addr_bytes[2] = (u8)((addr >> 16) & 0x000000FF);

	/*
	 * transfer : slave addresss
	 */
	data = chip<<1;
	pio_start(i2c);
	ret = pio_putbyte(i2c, data);
	if (ret) {
		printf("fail, i2c:%d start wait ack, addr:0x%02x \n", i2c->bus, data);
		goto __end_i2c_w;
	}

	/*
	 * transfer : regsiter addr
	 */
	while (--alen >= 0) {
		data = addr_bytes[alen];
		ret  = pio_putbyte(i2c, data);
		if (ret) {
			printf("Fail, i2c[%d] no ack data [0x%2x] \n", i2c->bus, data);
			goto __end_i2c_w;
		}
	}

	/*
	 * transfer : data
	 */
	while (len--) {
		data = *(buffer++);
		ret  = pio_putbyte(i2c, data);
		if (ret) {
			printf("Fail, i2c[%d] no ack data [0x%2x] \n", i2c->bus, data);
			goto __end_i2c_w;
		}
	}

#if (DBG_I2C)
	printf("i2c[%d]: W done chip=0x%2x \n", i2c->bus, chip);
#endif

__end_i2c_w:
	/*
	 * transfer : end
	 */
	if (ret || !no_stop)
		pio_stop(i2c);

	return ret;
}

int i2c_read (u8 chip, uint addr, int alen, u8 *buffer, int len)
{
	struct i2c_param *i2c = i2c_cur;
	u8  data;
	int ret  = -1;

	chip |= 0x01;

#if (DBG_I2C)
	printf("i2c[%d]: R chip=0x%2x, addr=0x%x, alen=%d, rlen=%d\n", i2c_bus, chip, addr, alen, len);
#endif

	if (NULL == i2c) {
		i2c_init(CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
		i2c = i2c_cur = &i2c_par[i2c_bus];
	}

	/*
	 * transfer : register addr
	 */
	if (0 > i2c_write(chip, addr, alen, NULL, 0))
		return ret;

	/*
	 * transfer : slave addresss
	 */
	data = chip<<1;
	data |= 0x1;
	pio_start(i2c);
	ret = pio_putbyte(i2c, data);
	if (ret) {
		printf("fail, i2c:%d start wait ack, addr:0x%02x \n", i2c->bus, data);
		goto __end_i2c_r;
	}

	/*
	 * transfer : read data
	 */
	while (len--) {
		int ack_gen = (len == 0) ? 0: 1;
		*buffer = pio_getbyte(i2c, ack_gen);
		 buffer++;
	}

__end_i2c_r:

	pio_stop(i2c);
	return ret;
}

void i2c_init(int speed, int slaveaddr)
{
	int i, io;

    NX_GPIO_Initialize();
    for (i = 0; NX_GPIO_GetNumberOfModule() > i; i++) {
        NX_GPIO_SetBaseAddress(i, (U32)IO_ADDRESS(NX_GPIO_GetPhysicalAddress(i)));
        NX_GPIO_OpenModule(i);
    }

	for (i = 0; NUMBER_OF_I2C_MODULE > i; i++) {
		io = i2c_par[0].scl_io;
		NX_GPIO_SetPadFunction(PAD_GET_GRP(io), PAD_GET_BIT(io), NX_GPIO_PADFUNC_GPIO);
		io = i2c_par[0].sda_io;
		NX_GPIO_SetPadFunction(PAD_GET_GRP(io), PAD_GET_BIT(io), NX_GPIO_PADFUNC_GPIO);
	}

    i2c_cur = &i2c_par[0];
    i2c_set_bus_speed(speed);
}

/*
 * params in:
 * 		chip = slave addres
 * return:
 *		0 = detect else not detect
 */
int i2c_probe(u8 chip)
{
	struct i2c_param *i2c = i2c_cur;
	u8  data;
	int ret  = -1;

	/* test with tx */
//	if (chip & 0x1)
//		return 01;

	if (NULL == i2c_cur){
		i2c_init(CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
		i2c = i2c_cur = &i2c_par[i2c_bus];
	}

	chip &= 0xFE;

	//DBGOUT("[%d]: chip=0x%2x\n", bus, chip);
	//printf("chip=0x%2x\n", chip);

	/*
	 * transfer : slave addresss
	 */
	data = chip<<1;

	pio_start(i2c);

	ret = pio_putbyte(i2c, data);

	pio_stop(i2c);

	return ret;
}


int do_i2c_mode (cmd_tbl_t *cmdtp, int flag, int argc, char * argv[])
{
	char *cmd;

	cmd = argv[1];
	if (strcmp(cmd, "stop") == 0) {
		printf("Set i2c bus %d stop mode  \n", i2c_bus);
		i2c_par[i2c_bus].no_stop = 0;
		return 0;
	} else if (strcmp(cmd, "nostop") == 0) {
		printf("Set i2c bus %d nostop mode \n", i2c_bus);
		i2c_par[i2c_bus].no_stop = 1;
		return 0;
	} else {
		printf("Current i2c bus %d %s mode  \n", i2c_bus, i2c_par[i2c_bus].no_stop?"nostop":"stop");
	}
	return 1;
}

U_BOOT_CMD(
	i2cmod, 3, 1,	do_i2c_mode,
	"set I2C mode",
	"stop\n"
	"    - generate stop signal, when tx end (normal)\n"
	"i2cmod nostop\n"
	"    - skip stop signal, when tx end (restart)\n"
);
