#!/bin/bash
SEND_IP=${1}

echo "Create Ramdisk Image 24M Size ..."
scp ramdisk_nxp2120_24M.gz $SEND_IP:/tmp/


