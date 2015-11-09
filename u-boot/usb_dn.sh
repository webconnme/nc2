#/bin/sh
nandbootec u-boot.bin.ec /opt/NSIH/DTK.700.350.175.087-192.NSIH.txt ./u-boot.bin 80100000 80100000
usb-downloader -t nxp2120 -n /opt/NSIH/DTK.700.350.175.087-192.NSIH.txt -f ./u-boot.bin.ec -a 81000000
