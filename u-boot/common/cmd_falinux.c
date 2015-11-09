/*

 커널이미지는 uImage 로처리하고
 ramdisk 는 uImage 가 아니기 때문에 직접 해당하는 주소에 넣는다.


 todo
 env 위치수정
 bootargs_ezb 생성
 bootcmd_ezb 생성
 load default 구현
 tfk, tfr, tfb, tfd, rst 명령어 생성

 */

#include <common.h>
#include <command.h>
#include <exports.h>
//#include <cli.h>

DECLARE_GLOBAL_DATA_PTR;
extern char console_buffer[];
extern int nxp_usbdn_size;  // arch/arm/cpu/arm1176/nxp2120/common/cmd_udown.c
extern int nxp_tftpdn_size;  // net/net.c

#define EZB_VER_STR     "0.1.2"

#if CONFIG_SYS_CBSIZE < 1024
#error "--------- need CONFIG_SYS_CBSIZE more then 1024"
#endif

enum {
	NAND_ADDR_BOOT							= 0x000000,
	NAND_ADDR_ENV								= 0x0e0000,
	NAND_ADDR_DT								= 0x0C0000,
	NAND_ADDR_KERNEL						= 0x200000,
	NAND_ADDR_RAMDISK						= 0xA00000,
	NAND_ADDR_RECOVERY_KERNEL		= 0x4800000,
	NAND_ADDR_RECOVERY_RAMDISK	= 0x5000000,
	NAND_ADDR_RECOVERY_APP			= 0x6000000,
	NAND_ADDR_APP_KERNEL				= 0xf800000, // add:lbb
};

#define DRAM_DOWNLOAD_BASE  0x81000000  //
#define DRAM_KERNEL_BOOTM   0x81000000
#define DRAM_RAMDISK_ADDR   0x82000000      // cmdline 과도 연결된다.
#define DRAM_DTREE_BOOTM    0x81F00000

#define KEY_LOAD_K      "ezb_load_k"
#define KEY_LOAD_K_A        "ezb_load_k_a" // add:lbb
#define KEY_LOAD_R      "ezb_load_r"
#define KEY_LOAD_DT     "ezb_load_dt"
#define KEY_BOOTM       "ezb_bootm"
#define KEY_BOOTARGS           "ezb_bootargs"
#define KEY_APP_BOOTARGS    "app_bootargs" // add:lbb
#define KEY_APP_KCMD2   "app_KCMD2"    // add:lbb
#define KEY_APP_BOOTM   "app_bootm"  // add:lbb
#define KEY_REC_SIZE_K      "ezb_rec_size_k"	// add larche
#define KEY_REC_SIZE_R      "ezb_rec_size_r"	// add larche
#define KEY_REC_SIZE_A      "ezb_rec_size_a"	// add larche

enum {
	F_IDX_ETHADDR = 0,
	F_IDX_IPADDR,
	F_IDX_NETMASK,
	F_IDX_GATEWAYIP,
	F_IDX_SERVERIP,
	F_IDX_KERNEL,
	F_IDX_RAMDISK,
	F_IDX_UBOOT,
	F_IDX_DT,
	F_IDX_AUTOEXEC,
	F_IDX_BOOTDELAY,
	F_IDX_KCMD1,
	F_IDX_KCMD2,
	F_IDX_KCMD3,
	F_IDX_KCMD4,
	F_IDX_KCMD5,
	F_IDX_BOOT_MODE, // add:lbb
	F_IDX_RECOVERY_APP, // add larche
	F_IDX_COUNT,
};

static char ezb_key[F_IDX_COUNT][32] = { "ethaddr", "ipaddr", "netmask",

"gatewayip", "serverip", "ezb_uImage", "ezb_ramdisk", "ezb_uboot", "ezb_dt",
		"ezb_autoexec", "bootdelay", "ezb_KCMD1", "ezb_KCMD2", "ezb_KCMD3",
		"ezb_KCMD4", "ezb_KCMD5", "boot_mode", // add:lbb
		"ezb_recovery_app", "", 
		};

static char ezb_def_str[F_IDX_COUNT + 1][80] = { "00:fa:15:21:20:07",
		"192.168.10.236", "255.255.255.0", "192.168.10.1", "192.168.10.61",
		"uImage", "ramdisk.wc-24M.gz", "uboot.nxp2120", "none", "/app/run.sh",
		"2", "console=ttyS0,115200",
		"root=/dev/ram0 rw initrd=0x82000000,12M ramdisk=24576",
		"earlyprintk", " ", " ", "env", // add lbb
		"webconn.img", // add larche
		"noinitrd root=/dev/mtdblock1 rw rootfstype=yaffs2",
		};

#define EZB_DEF_STR_RFS_RAMDISK_IDX     12
#define EZB_DEF_STR_RFS_NAND_IDX        F_IDX_COUNT

static char fmt_menu[] = "\nver %s (env-ofs=0x%x)\n\n"
		"  1) mac address              : %s\n"
		"  2) local ip                 : %s\n"
		"  3) local netmask            : %s\n"
		"  4) local gateway            : %s\n"
		"  5) server ip                : %s\n"
		"  6) uImage      file name    : %s\n"
		"  7) Ramdisk     file name    : %s\n"
		"  8) uBoot       file name    : %s\n"
		"  9) DeviceTree  file name    : %s\n"
		" 10) auto execute full name   : %s\n"
		" 11) autoboot wait time       : %s\n"
		" 12) KCMD1 : %s\n"
		" 13) KCMD2 : %s\n"
		" 14) KCMD3 : %s\n"
		" 15) KCMD4 : %s\n"
		" 16) KCMD5 : %s\n"
		" 17) boot mode : %s\n" // add:lbb
		"\n"
		"  L)  Load default               \n"
		"      LF) load cmdline root=flash  \n"
		"      LR) load cmdline root=ramdisk\n"
		"  M)  generlate Mac address      \n"
		"  S)  Save to flash              \n"
		"  Q)  Quit with apply            \n"
		"\n";

//------------------------------------------------------------------------------
/** @brief
 *///---------------------------------------------------------------------------
static char *get_ezb_strv(int f_idx) {
	char *pf;

	pf = getenv(ezb_key[f_idx]);
	if (NULL == pf) {
		setenv(ezb_key[f_idx], ezb_def_str[f_idx]);
		pf = getenv(ezb_key[f_idx]);
	}

	return pf;
}

//------------------------------------------------------------------------------
/** @brief
 *///---------------------------------------------------------------------------
static void setenv_common(void) {
	char str_env[CONFIG_SYS_CBSIZE];
	char *ptr; // add:lbb

	sprintf(str_env, "setenv bootargs ${%s} ${%s} mac=${ethaddr} ${%s} ${%s} ${%s}",
			ezb_key[F_IDX_KCMD1], ezb_key[F_IDX_KCMD2], ezb_key[F_IDX_KCMD3],
			ezb_key[F_IDX_KCMD4], ezb_key[F_IDX_KCMD5]);
	setenv( KEY_BOOTARGS, str_env);

	// add:lbb
	setenv(KEY_APP_KCMD2, "root=/dev/mtdblock8 rw yaffs");

	sprintf(str_env, "setenv bootargs ${%s} ${app_KCMD2} mac=${ethaddr} ${%s} ${%s} ${%s}",
			ezb_key[F_IDX_KCMD1], ezb_key[F_IDX_KCMD3], ezb_key[F_IDX_KCMD4],
			ezb_key[F_IDX_KCMD5]);
	setenv( KEY_APP_BOOTARGS, str_env);

	ptr = getenv(ezb_key[F_IDX_BOOT_MODE]);

	// boot mode
	if ((ptr == NULL) || (0 == strncmp(ptr, "env", 3))) {

		setenv("bootcmd", "run "KEY_BOOTARGS" "KEY_BOOTM);
	} else {

		setenv("bootcmd", "run "KEY_APP_BOOTARGS" "KEY_APP_BOOTM);
	}

	//--------------------------
}

//------------------------------------------------------------------------------
/** @brief
 *///---------------------------------------------------------------------------
static void rootfs_change(char mode) {
	char rfs_str[80];

	switch (mode) {
	case 'f':
	case 'F':
		sprintf(rfs_str, "%s", ezb_def_str[EZB_DEF_STR_RFS_NAND_IDX]);
		break;
	case 'r':
	case 'R':
		sprintf(rfs_str, "%s", ezb_def_str[EZB_DEF_STR_RFS_RAMDISK_IDX]);
		break;
	default:
		return;
	}

	setenv(ezb_key[F_IDX_KCMD2], rfs_str);
}

//------------------------------------------------------------------------------
/** @brief
 *///---------------------------------------------------------------------------
static int do_set(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]) {
	int nbytes, menu;

	//printf( "CONFIG_SYS_CBSIZE=%d\n", CONFIG_SYS_CBSIZE );
	setenv_common();

	while (1) {
		printf(fmt_menu, EZB_VER_STR, NAND_ADDR_ENV,
				get_ezb_strv(F_IDX_ETHADDR), get_ezb_strv(F_IDX_IPADDR),
				get_ezb_strv(F_IDX_NETMASK), get_ezb_strv(F_IDX_GATEWAYIP),
				get_ezb_strv(F_IDX_SERVERIP), get_ezb_strv(F_IDX_KERNEL),
				get_ezb_strv(F_IDX_RAMDISK), get_ezb_strv(F_IDX_UBOOT),
				get_ezb_strv(F_IDX_DT), get_ezb_strv(F_IDX_AUTOEXEC),
				get_ezb_strv(F_IDX_BOOTDELAY), get_ezb_strv(F_IDX_KCMD1),
				get_ezb_strv(F_IDX_KCMD2), get_ezb_strv(F_IDX_KCMD3),
				get_ezb_strv(F_IDX_KCMD4), get_ezb_strv(F_IDX_KCMD5),
				get_ezb_strv(F_IDX_BOOT_MODE)  // add:lbb
						);

		// 입력 대기
		nbytes = readline("  select> ");
		if (0 < nbytes) {
			char *ptr = console_buffer;

			switch (*ptr) {
			case 's':
			case 'S':
				saveenv();
				break;
			case 'q':
			case 'Q':
				return 0;
			case 'l':
			case 'L':
				rootfs_change(ptr[1]);
				break;
			}

			// 숫자 메뉴 확인
			menu = simple_strtoul(ptr, NULL, 0);
			menu -= 1;

			if ((0 <= menu) && (menu < F_IDX_COUNT)) {
				char buffer[CONFIG_SYS_CBSIZE];
				char *init_val;

				init_val = getenv(ezb_key[menu]);
				if (init_val)
					sprintf(buffer, "%s", init_val);
				else
					buffer[0] = '\0';

				if (0 <= readline_into_buffer("  edit> ", buffer)) {
					setenv(ezb_key[menu], buffer);
				}

				// add:lbb
				// boot mode
				if (menu == 16) {
					setenv_common();
				}
			}
		}
	}

	return 0;
}

U_BOOT_CMD(
		set, CONFIG_SYS_MAXARGS, 1, do_set,
		"set",
		"legacy ezboot command [set]"
);

//------------------------------------------------------------------------------
/** @brief
 *///---------------------------------------------------------------------------
#define NAND_ERASE_SIZE     0x20000
#define ALIGN_NAND_ERASE_ADDR(b)    (((b+NAND_ERASE_SIZE-1)>>17)<<17)

static int download_by_usb(char *fname) {
	char str_cmd[CONFIG_SYS_CBSIZE];

	sprintf(str_cmd, "udown 0x%x",
	DRAM_DOWNLOAD_BASE);
	printf("  >>%s\n", str_cmd);
	run_command(str_cmd, 0);

	return nxp_usbdn_size;
}

static int download_by_tftp(char *fname) {
	char str_cmd[CONFIG_SYS_CBSIZE];

	sprintf(str_cmd, "tftp 0x%x %s", 
	DRAM_DOWNLOAD_BASE,fname);
	printf("\n>>%s\n", str_cmd);
	run_command(str_cmd, 0);

	return nxp_tftpdn_size;
}

//------------------------------------------------------------------------------
/** @brief
 nand write addr size
 *///---------------------------------------------------------------------------
static void write_to_nand(int addr, int size) {
	char str_cmd[CONFIG_SYS_CBSIZE];

	sprintf(str_cmd, "nand erase 0x%x 0x%x", addr, size);
	printf("\n>>%s", str_cmd);
	run_command(str_cmd, 0);

	sprintf(str_cmd, "nand write 0x%x 0x%x 0x%x",
	DRAM_DOWNLOAD_BASE, addr, size);
	printf("\n>>%s", str_cmd);
	run_command(str_cmd, 0);
}

//------------------------------------------------------------------------------
/** @brief
 *///---------------------------------------------------------------------------
static void saveenv_after_download(void) {
	char str_env[CONFIG_SYS_CBSIZE];
	char *ptr;

	ptr = getenv(ezb_key[F_IDX_DT]);

	// not used FDT
	if ((ptr == NULL) || (0 == strncmp(ptr, "none", 4))) {
		// env, bootm
		sprintf(str_env, "%s; %s; bootm 0x%x", getenv( KEY_LOAD_K),
				getenv( KEY_LOAD_R),
				DRAM_KERNEL_BOOTM);
	}
	// used FDT
	else {
		// env, bootm
		sprintf(str_env, "%s; %s; %s; bootm 0x%x - 0x%x", getenv( KEY_LOAD_K),
				getenv( KEY_LOAD_R), getenv( KEY_LOAD_DT),
				DRAM_KERNEL_BOOTM,
				DRAM_DTREE_BOOTM);
	}

	setenv( KEY_BOOTM, str_env);
	setenv_common();
	saveenv();
}

//------------------------------------------------------------------------------
/** @brief
 *///---------------------------------------------------------------------------
static int do_ezb_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]) {
	char str_env[CONFIG_SYS_CBSIZE];
	int fsize, ssize;

	if (0 == strcmp("rst", argv[0])) {
		run_command("reset", 0);
	}
	if (0 == strcmp("gk", argv[0])) {
		run_command("run bootcmd", 0);
	} else if (0 == strcmp("ufk", argv[0])) {
		// download and write
		fsize = download_by_usb(get_ezb_strv(F_IDX_KERNEL));
		ssize = ALIGN_NAND_ERASE_ADDR(fsize);
		write_to_nand(NAND_ADDR_KERNEL, ssize);

		// env, load kernel
		sprintf(str_env, "nand read 0x%x 0x%x 0x%x", DRAM_KERNEL_BOOTM,
				NAND_ADDR_KERNEL, ssize);
		setenv( KEY_LOAD_K, str_env);

		// save env
		saveenv_after_download();
	} else if (0 == strcmp("ufka", argv[0]))   // add:lbb
			{

		// download and write
		fsize = download_by_usb(get_ezb_strv(F_IDX_KERNEL));
		ssize = ALIGN_NAND_ERASE_ADDR(fsize);
		write_to_nand(NAND_ADDR_APP_KERNEL, ssize);

		// env, load kernel
		sprintf(str_env, "nand read 0x%x 0x%x 0x%x", DRAM_KERNEL_BOOTM,
				NAND_ADDR_APP_KERNEL, ssize);
		setenv( KEY_LOAD_K_A, str_env);

		// save env
		sprintf(str_env, "%s; %s; bootm 0x%x", getenv( KEY_LOAD_K_A),
				getenv( KEY_LOAD_R),
				DRAM_KERNEL_BOOTM);
		setenv( KEY_APP_BOOTM, str_env);
		setenv_common();
		saveenv();

	} else if (0 == strcmp("ufr", argv[0])) {
		// download and write
		fsize = download_by_usb(get_ezb_strv(F_IDX_RAMDISK));
		ssize = ALIGN_NAND_ERASE_ADDR(fsize);
		write_to_nand(NAND_ADDR_RAMDISK, ssize);

		// env, load ramdisk
		sprintf(str_env, "nand read 0x%x 0x%x 0x%x", DRAM_RAMDISK_ADDR,
				NAND_ADDR_RAMDISK, ssize);
		setenv( KEY_LOAD_R, str_env);

		// save env
		saveenv_after_download();
	} else if (0 == strcmp("ufd", argv[0])) {
		// download and write
		fsize = download_by_usb(get_ezb_strv(F_IDX_DT));
		ssize = ALIGN_NAND_ERASE_ADDR(fsize);
		write_to_nand(NAND_ADDR_DT, ssize);

		// env, load dt
		sprintf(str_env, "nand read 0x%x 0x%x 0x%x", DRAM_DTREE_BOOTM,
				NAND_ADDR_DT, ssize);
		setenv( KEY_LOAD_DT, str_env);

		// save env
		saveenv_after_download();
	} else if (0 == strcmp("ufb", argv[0])) {
		fsize = download_by_usb(get_ezb_strv(F_IDX_UBOOT));
		ssize = ALIGN_NAND_ERASE_ADDR(fsize);

		write_to_nand(NAND_ADDR_BOOT, ssize);
	} else if (0 == strcmp("tfb", argv[0])) {
		fsize = download_by_tftp(get_ezb_strv(F_IDX_UBOOT));
		ssize = ALIGN_NAND_ERASE_ADDR(fsize);

		write_to_nand(NAND_ADDR_BOOT, ssize);
	} else if (0 == strcmp("tfr", argv[0])) {
		// download and write
		fsize = download_by_tftp(get_ezb_strv(F_IDX_RAMDISK));
		ssize = ALIGN_NAND_ERASE_ADDR(fsize);
		write_to_nand(NAND_ADDR_RAMDISK, ssize);

		// env, load ramdisk
		sprintf(str_env, "nand read 0x%x 0x%x 0x%x", DRAM_RAMDISK_ADDR,
				NAND_ADDR_RAMDISK, ssize);
		setenv( KEY_LOAD_R, str_env);

		// save env
		saveenv_after_download();
	}	else if (0 == strcmp("tfk", argv[0])) {
		// download and write
		fsize = download_by_tftp(get_ezb_strv(F_IDX_KERNEL));
		ssize = ALIGN_NAND_ERASE_ADDR(fsize);
		write_to_nand(NAND_ADDR_KERNEL, ssize);

		// env, load kernel
		sprintf(str_env, "nand read 0x%x 0x%x 0x%x", DRAM_KERNEL_BOOTM,
				NAND_ADDR_KERNEL, ssize);
		setenv( KEY_LOAD_K, str_env);

		// save env
		saveenv_after_download();
	} else if (0 == strcmp("rtfr", argv[0])) {
		// download and write
		fsize = download_by_tftp(get_ezb_strv(F_IDX_RAMDISK));
		ssize = ALIGN_NAND_ERASE_ADDR(fsize);
		write_to_nand(NAND_ADDR_RECOVERY_RAMDISK, ssize);

		sprintf(str_env, "0x%x", ssize);
		setenv( KEY_REC_SIZE_R, str_env);
		saveenv_after_download();

	}	else if (0 == strcmp("rtfk", argv[0])) {
		// download and write
		fsize = download_by_tftp(get_ezb_strv(F_IDX_KERNEL));
		ssize = ALIGN_NAND_ERASE_ADDR(fsize);
		write_to_nand(NAND_ADDR_RECOVERY_KERNEL, ssize);

		sprintf(str_env, "0x%x", ssize);
		setenv( KEY_REC_SIZE_K, str_env);
		saveenv_after_download();

	} else if (0 == strcmp("rtfa", argv[0])) {
		// download and write
		fsize = download_by_tftp(get_ezb_strv(F_IDX_RECOVERY_APP));
		ssize = ALIGN_NAND_ERASE_ADDR(fsize);
		write_to_nand(NAND_ADDR_RECOVERY_APP, ssize);

		sprintf(str_env, "0x%x", ssize);
		setenv( KEY_REC_SIZE_A, str_env);
		saveenv_after_download();

	}

	return 0;
}

U_BOOT_CMD(
		ufk, CONFIG_SYS_MAXARGS, 0, do_ezb_cmd,
		"ufk",
		"download <uimage> by usb then write on flash"
);

U_BOOT_CMD( // add:lbb
		ufka, CONFIG_SYS_MAXARGS, 0, do_ezb_cmd,
		"ufka",
		"download <uimage> by usb then write on flash"
);

U_BOOT_CMD(
		ufr, CONFIG_SYS_MAXARGS, 0, do_ezb_cmd,
		"ufr",
		"download <ramdisk> by usb then write on flash"
);

U_BOOT_CMD(
		ufd, CONFIG_SYS_MAXARGS, 0, do_ezb_cmd,
		"ufd",
		"download <dt> by usb then write on flash"
);

U_BOOT_CMD(
		ufb, CONFIG_SYS_MAXARGS, 0, do_ezb_cmd,
		"ufb for bootloader",
		"download <uboot> by usb then write on flash"
);

U_BOOT_CMD(
		tfb, CONFIG_SYS_MAXARGS, 0, do_ezb_cmd,
		"tfb for bootloader",
		"download <uboot> by tftp then write on flash"
);

U_BOOT_CMD(
		tfr, CONFIG_SYS_MAXARGS, 0, do_ezb_cmd,
		"tfr for ramdisk",
		"download <ramdisk> by usb then write on flash"
);

U_BOOT_CMD(
		tfk, CONFIG_SYS_MAXARGS, 0, do_ezb_cmd,
		"tfk for kernel",
		"download <uimage> by usb then write on flash"
);

U_BOOT_CMD(
		rtfr, CONFIG_SYS_MAXARGS, 0, do_ezb_cmd,
		"rtfr for recovery ramdisk",
		"download <ramdisk> by usb then write on flash"
);

U_BOOT_CMD(
		rtfk, CONFIG_SYS_MAXARGS, 0, do_ezb_cmd,
		"rtfk for recovery kernel",
		"download <uimage> by usb then write on flash"
);

U_BOOT_CMD(
		rtfa, CONFIG_SYS_MAXARGS, 0, do_ezb_cmd,
		"rtfa for recovery app",
		"download <appimage> by usb then write on flash"
);

U_BOOT_CMD(
		rst, CONFIG_SYS_MAXARGS, 0, do_ezb_cmd,
		"rst",
		"reset"
);

U_BOOT_CMD(
		gk, CONFIG_SYS_MAXARGS, 0, do_ezb_cmd,
		"gk",
		"go kernel"
);

/*
 U_BOOT_CMD(name,maxargs,repeatable,command,"usage","help")

 - name          is the name of the commad. THIS IS NOT a string.
 - maxargs       the maximumn numbers of arguments this function takes
 - repeatable
 - command       Function pointer (*cmd)(struct cmd_tbl_s *, int, int, char *[]);
 - usage         Short description. This is a string
 - help          long description. This is a string
 */

