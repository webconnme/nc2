/*

*/


#include <common.h>
#include <command.h>
#include <exports.h>
//#include <cli.h>

DECLARE_GLOBAL_DATA_PTR;

#define    DRAM_START         0x80000000
// #define    DRAM_SIZE          (512*1024*1024)
#define    DRAM_SIZE          (256*1024*1024)
#define    DRAM_END           (DRAM_START+DRAM_SIZE-1)        
//#define    UBOOT_AREA_START   (CFG_BLD_BOOT_STACK_TOP - CONFIG_STACKSIZE)
#define    UBOOT_AREA_START   DRAM_START
#define    UBOOT_AREA_END     (CFG_KERNEL_TEXT_BASE-1)
#define    UBOOT_AREA_SIZE    (UBOOT_AREA_END-UBOOT_AREA_START+1)

#define SIZE_1M               (1024*1024)

static void clear_1M_block( unsigned long * ptr_block, unsigned long pattern ){
	
    int lp;
	unsigned long *ptr;
	
	ptr = ptr_block;
	
	for( lp = 0; lp < (SIZE_1M/4); lp++ ){
		*ptr = pattern;
		ptr++;
	}

}

static int do_zmac_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]){

    int          lp_1M;
    unsigned long *ptr_1M;
    unsigned long pattern;
	
    printf( "start to clear all memory for zeroboot...\n" );	
	
	// board/nxp2120/dtk/include/cfg_mem.h
	printf( "dram       START        : %08X\n", DRAM_START );
	printf( "dram       END          : %08X\n", DRAM_END   );
	printf( "dram       SIZE         : %08X\n", DRAM_SIZE );
	
	printf( "uBoot area START        : %08X\n", UBOOT_AREA_START );
	printf( "uBoot area END          : %08X\n", UBOOT_AREA_END   );
	printf( "uBoot area SIZE         : %08X\n", UBOOT_AREA_SIZE  );
	
	pattern = 0x00000000;
	
//	printf( "argc : %d\n", argc );
	if( argc == 2 ) {
//		printf( "argv[1] : %s\n", argv[1] );
		pattern = simple_strtoul( argv[1], NULL, 0 );
		printf( "pattern : %08X\n", pattern );
	}	
	
	
	ptr_1M = (unsigned long *) DRAM_START;
	
	for( lp_1M = 0; lp_1M < (DRAM_SIZE/SIZE_1M); lp_1M++ ){

		printf( "\r%08X:S-%08X/E-%08X", ptr_1M, DRAM_START, DRAM_END );
//		printf( "%08X:S-%08X/E-%08X", ptr_1M, DRAM_START, DRAM_END );
	    if( ( ptr_1M >= ((unsigned long *) UBOOT_AREA_START )) 
		&&  ( ptr_1M <= ((unsigned long *) UBOOT_AREA_END   )) ){
			// SKIP
//			printf( " - SKIP\n" );
		} else {
		    clear_1M_block( ptr_1M, pattern );	
//			printf( " - CLEAR\n" );
        }			
	
	    ptr_1M += (SIZE_1M/4);
		
	}
	printf( "\nALL CLEAR END..............\n" );
   
	return 0;
}

/*
U_BOOT_CMD(name,maxargs,repeatable,command,"usage","help")

	- name			is the name of the commad. THIS IS NOT a string.
	- maxargs		the maximumn numbers of arguments this function takes
	- repeatable
	- command		Function pointer (*cmd)(struct cmd_tbl_s *, int, int, char *[]);
	- usage			Short description. This is a string
	- help			long description. This is a string
*/

U_BOOT_CMD(
	zmac,							// name
	CONFIG_SYS_MAXARGS,				// maxargs
	0,	                            // repeatable
	do_zmac_cmd,                    // command  
	"zmac [32 bit pattern]",        // usage
	"clear all memory for zeroboot" // help 
);


