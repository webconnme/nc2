/*
 */

/*
 * Misc boot support
 */
#include <common.h>
#include <command.h>
#include <malloc.h>
#include <nand.h>

#include <mach-types.h>

DECLARE_GLOBAL_DATA_PTR;

typedef void  (IMAGE)(unsigned long, unsigned long);

#if defined(CONFIG_SETUP_MEMORY_TAGS) || \
	defined(CONFIG_CMDLINE_TAG) || \
	defined(CONFIG_INITRD_TAG) || \
	defined(CONFIG_SERIAL_TAG) || \
	defined(CONFIG_REVISION_TAG)
static struct tag *params;
#endif

#if defined(CONFIG_SETUP_MEMORY_TAGS) || \
	defined(CONFIG_CMDLINE_TAG) || \
	defined(CONFIG_INITRD_TAG) || \
	defined(CONFIG_SERIAL_TAG) || \
	defined(CONFIG_REVISION_TAG)
static void setup_start_tag (bd_t *bd)
{
	params = (struct tag *)bd->bi_boot_params;

	params->hdr.tag = ATAG_CORE;
	params->hdr.size = tag_size (tag_core);

	params->u.core.flags = 0;
	params->u.core.pagesize = 0;
	params->u.core.rootdev = 0;

	params = tag_next (params);
}
#endif

#ifdef CONFIG_SETUP_MEMORY_TAGS
static void setup_memory_tags(bd_t *bd)
{
	int i;

	for (i = 0; i < CONFIG_NR_DRAM_BANKS; i++) {
		params->hdr.tag = ATAG_MEM;
		params->hdr.size = tag_size (tag_mem32);

		params->u.mem.start = bd->bi_dram[i].start;
		params->u.mem.size = bd->bi_dram[i].size;

		params = tag_next (params);
	}
}
#endif

#ifdef CONFIG_CMDLINE_TAG
static void setup_commandline_tag(bd_t *bd, char *commandline)
{
	char *p;

	if (!commandline)
		return;

	/* eat leading white space */
	for (p = commandline; *p == ' '; p++);

	/* skip non-existent command lines so the kernel will still
	 * use its default command line.
	 */
	if (*p == '\0')
		return;

	params->hdr.tag = ATAG_CMDLINE;
	params->hdr.size =
		(sizeof (struct tag_header) + strlen (p) + 1 + 4) >> 2;

	strcpy (params->u.cmdline.cmdline, p);

	params = tag_next (params);
}
#endif

#ifdef CONFIG_INITRD_TAG
static void setup_initrd_tag(bd_t *bd, ulong initrd_start, ulong initrd_end)
{
	/* an ATAG_INITRD node tells the kernel where the compressed
	 * ramdisk can be found. ATAG_RDIMG is a better name, actually.
	 */
	params->hdr.tag = ATAG_INITRD2;
	params->hdr.size = tag_size (tag_initrd);

	params->u.initrd.start = initrd_start;
	params->u.initrd.size = initrd_end - initrd_start;

	params = tag_next (params);
}
#endif

#ifdef CONFIG_SERIAL_TAG
void setup_serial_tag(struct tag **tmp)
{
	struct tag *params = *tmp;
	struct tag_serialnr serialnr;
	void get_board_serial(struct tag_serialnr *serialnr);

	get_board_serial(&serialnr);
	params->hdr.tag = ATAG_SERIAL;
	params->hdr.size = tag_size (tag_serialnr);
	params->u.serialnr.low = serialnr.low;
	params->u.serialnr.high= serialnr.high;
	params = tag_next (params);
	*tmp = params;
}
#endif

#ifdef CONFIG_REVISION_TAG
void setup_revision_tag(struct tag **in_params)
{
	u32 rev = 0;
	u32 get_board_rev(void);

	rev = get_board_rev();
	params->hdr.tag = ATAG_REVISION;
	params->hdr.size = tag_size (tag_revision);
	params->u.revision.rev = rev;
	params = tag_next (params);
}
#endif

#if defined(CONFIG_SETUP_MEMORY_TAGS) || \
	defined(CONFIG_CMDLINE_TAG) || \
	defined(CONFIG_INITRD_TAG) || \
	defined(CONFIG_SERIAL_TAG) || \
	defined(CONFIG_REVISION_TAG)
static void setup_end_tag(bd_t *bd)
{
	params->hdr.tag = ATAG_NONE;
	params->hdr.size = 0;
}
#endif

int do_zimage (cmd_tbl_t *cmdtp, int flag, int argc, char * argv[])
{
	ulong addr = 0;
	long  machtype = MACH_TYPE_NXP2120;
#ifdef CONFIG_CMDLINE_TAG
	char *commandline = getenv("bootargs");
#endif

	void (*entry)(unsigned long, unsigned long) = NULL;

	if (argc < 2) {
		cmd_usage(cmdtp);
		return 1;
	}

	/* get machine type */
	if (argc == 3)
		machtype = simple_strtol(argv[2], NULL, 10);	/* get interger machine type */

	/* get start address */
	addr = simple_strtoul(argv[1], NULL, 16);

#if defined(CONFIG_SETUP_MEMORY_TAGS) || \
	defined(CONFIG_CMDLINE_TAG) || \
	defined(CONFIG_INITRD_TAG) || \
	defined(CONFIG_SERIAL_TAG) || \
	defined(CONFIG_REVISION_TAG)

	setup_start_tag(gd->bd);

	#ifdef CONFIG_SERIAL_TAG
	setup_serial_tag(&params);
	#endif
	#ifdef CONFIG_CMDLINE_TAG
	setup_commandline_tag(gd->bd, commandline);
	#endif
	#ifdef CONFIG_REVISION_TAG
	setup_revision_tag(&params);
	#endif
	#ifdef CONFIG_SETUP_MEMORY_TAGS
	setup_memory_tags(gd->bd);
	#endif
	#ifdef CONFIG_INITRD_TAG
	if (images->rd_start && images->rd_end)
		setup_initrd_tag(gd->bd, images->rd_start,
		images->rd_end);
	#endif
	setup_end_tag(gd->bd);
#endif

	printf ("## Starting Image at 0x%08X with machine type %d (tags=0x%08x)...\n",
		(u_int)addr, (u_int)machtype, (u_int)gd->bd->bi_boot_params);

	entry = (IMAGE*)addr;

	entry(addr, machtype);

	return 0;
}

U_BOOT_CMD(
	zimage, CONFIG_SYS_MAXARGS, 1,	do_zimage,
	"start Image at address 'addr'",
	"addr\n"
	"    - start Image at address 'addr' with default machine type\n"
	"addr type\n"
	"    - start Image at address 'addr' with machine type integer\n"
);

int do_info(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	char *cmd;

	/* at least two arguments please */
	if (argc < 2)
		goto usage;

	cmd = argv[1];
#if defined(CONFIG_CMD_NAND)
	if (strcmp(cmd, "nand") == 0) {
		int i;
		for (i = 0; i < CONFIG_SYS_MAX_NAND_DEVICE; i++) {
			if (nand_info[i].name) {
	    		nand_info_t *nand = &nand_info[i];
    			struct nand_chip *chip = nand->priv;
    			struct mtd_info *mtd = nand;
    			u8 id_data[8];
    			printf("Device %d: ", i);
	    		if (chip->numchips > 1)
    	    		printf("%dx ", chip->numchips);
    			printf("%s\n", nand->name);
    			printf(" oob    size: %4u Byte\n", nand->oobsize);
    			if (nand->writesize >= (1<<10))
    				printf(" page   size: %4u KiB \n", nand->writesize >> 10);
    			else
    				printf(" page   size: %4u Byte\n", nand->writesize);
    			printf(" sector size: %4u KiB \n", nand->erasesize >> 10);
				printf("\n");

				/* Read entire ID string */
				chip->select_chip(mtd, 0);
    			chip->cmdfunc(mtd, NAND_CMD_READID, 0x00, -1);

				for (i = 0; i < 8; i++)
					id_data[i] = chip->read_byte(mtd);

				for (i = 0; i < 8; i++)
					printf(" iddata %d th: 0x%x\n", i+1, id_data[i]);

    		}
		}
		return 0;
	}
#endif
usage:
	cmd_usage(cmdtp);
	return 1;
}

U_BOOT_CMD(
	info, CONFIG_SYS_MAXARGS, 1,	do_info,
	"show device info",
#if defined(CONFIG_CMD_NAND)
	"nand\n"
	"    - show nand device info\n"
#endif
);

