#include <common.h>
#include <command.h>

extern int run_command (const char *cmd, int flag);
extern int usb_stor_curr_dev;

static int do_update(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    int ret;

    ret = run_command("usb start", 0);
    if (ret < 0 || usb_stor_curr_dev < 0) {
        printf("update nand error: usb scan failed... retry!\n");
        return ret;
    }

    ret = run_command("fatload usb 0 81000000 u-boot-2121-nand.bin", 0);
    if (ret < 0) {
        printf("can't find u-boot-2121-nand.bin, skip u-boot update\n");
    } else {
        ret = run_command("nand erase 0 40000", 0);
        if (ret < 0) {
            printf("update nand error: nand erase failed\n");
            return ret;
        }
        ret = run_command("nand write 81000000 0 40000", 0);
        if (ret < 0) {
            printf("update nand error: nand write failed\n");
            return ret;
        }
    }

    ret = run_command("fatload usb 0 81000000 uImage-vstation", 0);
    if (ret < 0) {
        printf("can't find uImage-vstation, skip kernel update\n");
    } else {
        ret = run_command("nand erase 100000 200000", 0);
        if (ret < 0) {
            printf("update nand error: nand erase failed\n");
            return ret;
        }
        ret = run_command("nand write 81000000 100000 200000", 0);
        if (ret < 0) {
            printf("update nand error: nand write failed\n");
            return ret;
        }
    }

    ret = run_command("fatload usb 0 81000000 rootfs-vstation.img.gz", 0);
    if (ret < 0) {
        printf("can't find rootfs-vstation.img.gz, skip rootfs update\n");
    } else {
        ret = run_command("nand erase 300000 200000", 0);
        if (ret < 0) {
            printf("update nand error: nand erase failed\n");
            return ret;
        }
        ret = run_command("nand write 81000000 300000 200000", 0);
        if (ret < 0) {
            printf("update nand error: nand write failed\n");
            return ret;
        }
    }

    setenv("bootcmd", "nand read 81000000 100000 160000;nand read 82000000 300000 1e0000;bootm 81000000");
    setenv("bootargs", "console=ttyS0,115200n8 root=/dev/ram0 rootfstype=ext2 ramdisk_size=8192 initrd=0x82000000,8M printk.time=1");

    ret = run_command("saveenv", 0);
    if (ret < 0) {
        printf("saveenv failed\n");
    }

    printf("update nand success!!!\n");
    printf("set your board to nand boot mode & reboot\n");
    return 0;
}

U_BOOT_CMD(
        update, CONFIG_SYS_MAXARGS, 1, do_update,
        "update vstation nand",
        ""
        );
