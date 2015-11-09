#
# (C) Copyright 2010
# jung hyun kim, Nexell Co, <jhkim@nexell.co.kr>
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

include $(TOPDIR)/config.mk
LIB 	= $(obj)lib$(SOC)-devices.o

COBJS-y	:= serial.o 

COBJS-$(CONFIG_GENERIC_MMC)		+= sdhc.o
COBJS-$(CONFIG_CMD_NAND) 		+= nand.o
COBJS-$(CONFIG_SYS_NAND_HW_ECC) += nand_ecc.o
COBJS-$(CONFIG_CMD_I2C) 		+= i2c.o
COBJS-$(CONFIG_NXE1000_POWER) 	+= nxe1000_power.o

COBJS   := $(COBJS-y)
SOBJS   := $(SOBJS-y)

SRCS	:= $(SOBJS:.o=.S) $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(SOBJS) $(COBJS))

all:	$(obj).depend $(LIB)

$(LIB): $(OBJS)
	$(AR) $(ARFLAGS) $@  $(OBJS)

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
