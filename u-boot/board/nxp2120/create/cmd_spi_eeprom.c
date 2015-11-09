/*
 * (C) Copyright 2000-2003
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
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

/*
 * Misc boot support
 */
#include <common.h>
#include <command.h>
#include <spi.h>

struct BOOTINFO {
	int				loadsize;
	unsigned int	loadaddr;
	unsigned int	jumpaddr;
};

int parse_nsih(char *addr, int size)
{
	char ch;
	int writesize, skipline, line, bytesize, i;
	unsigned int writeval;

	struct BOOTINFO *pinfo = NULL;
	char *base = addr;
	char  buffer[512] = { 0, };

	bytesize  = 0;
	writeval  = 0;
	writesize = 0;
	skipline  = 0;
	line = 0;

	while (1) {

		ch = *addr++;
		if (0 >= size)
			break;

		if (skipline == 0) {
			if (ch >= '0' && ch <= '9') {
				writeval  = writeval * 16 + ch - '0';
				writesize += 4;
			} else if (ch >= 'a' && ch <= 'f') {
				writeval  = writeval * 16 + ch - 'a' + 10;
				writesize += 4;
			} else if (ch >= 'A' && ch <= 'F') {
				writeval  = writeval * 16 + ch - 'A' + 10;
				writesize += 4;
			} else {
				if (writesize == 8 || writesize == 16 || writesize == 32) {
					for (i=0 ; i<writesize/8 ; i++) {
						buffer[bytesize++] = (unsigned char)(writeval & 0xFF);
						writeval >>= 8;
					}
				} else {
					if (writesize != 0)
						printf("parse nsih : Error at %d line.\n", line+1);
				}

				writesize = 0;
				skipline = 1;
			}
		}

		if (ch == '\n') {
			line++;
			skipline = 0;
			writeval = 0;
		}

		size--;
	}

	pinfo = (struct BOOTINFO *)&buffer[4];

	pinfo->loadsize	= (int)CONFIG_UBOOT_SIZE;
	pinfo->loadaddr	= (U32)_armboot_start;
	pinfo->jumpaddr = (U32)_armboot_start;

	memcpy(base, buffer, sizeof(buffer));

	printf(" parse nsih : %d line processed\n", line+1);
	printf(" parse nsih : %d bytes generated.\n\n", bytesize);

	return bytesize;
}

int do_update(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	char *cmd;
	unsigned int addr = 0, offset = 0;
	U8 offs[3] = { 0, };
	int size = 0, maxsize = 0;

	if (4 > argc)
		goto usage;

	cmd = argv[1];

	if (0 != strcmp(cmd, "1stboot") &&
		0 != strcmp(cmd, "nsih")    &&
		0 != strcmp(cmd, "uboot") )
		goto usage;

	addr = simple_strtoul(argv[2], NULL, 16);
	size = simple_strtoul(argv[3], NULL, 16);

	if (strcmp(cmd, "1stboot") == 0) {
		offset  = CONFIG_1STBOOT_OFFSET;
		maxsize = CONFIG_1STBOOT_SIZE;

		printf(" eeprom update 1stboot 0x%08x, mem 0x%08x, size %d \n", offset, addr, size);
	}
	else if (strcmp(cmd, "nsih") == 0) {
		offset  = CONFIG_NSIH_OFFSET;
		maxsize = CONFIG_NSIH_SIZE;
		size    = parse_nsih((char*)addr, size);

		printf(" eeprom update nsih 0x%08x, mem 0x%08x, size %d \n", offset, addr, size);
		if (512 != size) {
			printf(" fail nsih parse, invalid nsih headers ...\n");
			return 1;
		}
	}
	else if (strcmp(cmd, "uboot") == 0) {
		maxsize = CONFIG_UBOOT_SIZE;
		offset  = CONFIG_UBOOT_OFFSET;

		printf(" eeprom update u-boot 0x%08x, mem 0x%08x, size %d\n", offset, addr, size);
	} else {
		cmd_usage(cmdtp);
		return 1;
	}

	if (size > maxsize) {
		printf(" error input size %d is over the max part size %d\n", size, maxsize);
		goto usage;
	}

	offs[0] = (offset >>  16);
	offs[1] = (offset >>   8);
	offs[2] = (offset & 0xFF);

	spi_write(offs, 0, (uchar*)addr, size);
	return 0;

usage:
	cmd_usage(cmdtp);
	return 1;
}

U_BOOT_CMD(
	update, CONFIG_SYS_MAXARGS, 1,	do_update,
	"update eeprom data",
	"uboot addr cnt (hex)\n"
	"    - update uboot loder (cnt max 240 Kbyte)\n"
	"nsih addr cnt (hex)\n"
	"    - update nsih file (cnt max 4 Kbyte)\n"
	"1stboot addr cnt (hex)\n"
	"    - update 1st boot loader (cnt max 4 Kbyte)\n"
);
