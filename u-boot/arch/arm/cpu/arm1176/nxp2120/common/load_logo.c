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

#ifndef CONFIG_BOOTLOGO_COMMAND
#define	CONFIG_BOOTLOGO_COMMAND 	NULL
#endif

static unsigned int __logo_base = 0x80000000;

extern int run_command (const char *cmd, int flag);

int parse_logo_cmd (char *line, char *argv[])
{
	int nargs = 0;

	while (nargs < CONFIG_SYS_MAXARGS) {

		/* skip any white space */
		while ((*line == ' ') || (*line == '\t') || (*line == ';')) {
			++line;
		}

		if (*line == '\0') {	/* end of line, no more args	*/
			argv[nargs] = NULL;
			return (nargs);
		}

		argv[nargs++] = line;	/* begin of argument string	*/

		/* find end of string */
		while (*line && (*line != ';') && (*line != '\t')) {
			++line;
		}

		if (*line == '\0') {	/* end of line, no more args	*/
			argv[nargs] = NULL;
			return (nargs);
		}

		*line++ = '\0';		/* terminate current arg	 */
	}

	printf ("** Too many args (max. %d) **\n", CONFIG_SYS_MAXARGS);
	return (nargs);
}


unsigned int logo_get_base(void)
{
	return __logo_base;
}

void logo_set_base(unsigned int addr)
{
	__logo_base = addr;
}

int load_boot_logo(void)
{
	char *default_command = CONFIG_BOOTLOGO_COMMAND;
	char  cmd[128] = { 0 };
	char *s = NULL;
	char *argv[CONFIG_SYS_MAXARGS + 1]; /* NULL terminated  */
	int argc = 0;
	unsigned int addr = 0;
	int i;

	if (NULL == (s = getenv ("bootlogo")))
		s = default_command;

	if (s) {
		strcpy(cmd, s);
		argc = parse_logo_cmd(cmd, argv);
	}

#ifdef DEBUG_PARSER
		printf("bootlogo = %s\n", s);
#endif

	if (2 > argc)
		return 1;

	for (i=0; argc > i; ++i) {
		char *arg = argv[i];
#ifdef DEBUG_PARSER
		printf("logo arg = %s\n", arg);
#endif
		if (strncmp(arg, "bootlogo", strlen("bootlogo")) == 0) {
			char *arg2 = arg + strlen("bootlogo");

			while (*arg2 == ' ') { arg2++; }

			if ((*arg2 == '\t') ||
				(*arg2 == ';')  ||
				(*arg2 == '\0'))
				continue;

			addr = simple_strtoul(arg2, NULL, 16);
			if (addr)
				logo_set_base(addr);
			continue;
		}
		if (-1 == run_command (arg, 0))
			return 1;
	}
#ifdef DEBUG_PARSER
		printf("done bootlogo = %s\n", s);
#endif
	return 0;
}

