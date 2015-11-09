#!/bin/sh

make distclean

if [ ! -f include/autoconf.mk ]; then
	make ARCH=arm CROSS_COMPILE=/opt/crosstools/arm-eabi-4.6.3-glibc-2.13/bin/arm-generic-linux-gnueabi- nxp2120_dtk_config
fi

make ARCH=arm CROSS_COMPILE=/opt/crosstools/arm-eabi-4.6.3-glibc-2.13/bin/arm-generic-linux-gnueabi- $1

if [ -f u-boot.bin ]; then
	./nandbootec u-boot.bin.ec ./DTK.700.350.175.087-192.NSIH.txt ./u-boot.bin 80100000 80100000
fi

if [ -f u-boot.bin.ec ]; then
	sudo cp -a u-boot.bin.ec /tftpboot/uboot.nxp2120
fi	

#scp u-boot.bin.ec u-boot.bin falbb@192.168.2.130:/share/work/nxp2120/7week

