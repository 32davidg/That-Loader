#include "../include/configfile.h"


// config file path
#define CFG_PATH ("\\EFI\\thatloader\\config.cfg")

#define MAX_ENTRY_NAME_LEN (70)

#define LINUX_KERNEL_IDENTIFIER_STR ("vmlinuz")
#define STR_TO_SUBSTITUTE_WITH_VERSION ("%v")

#define CFG_LINE_DELIMITER      ("\n")
#define CFG_ENTRY_DELIMITER     ("\n\n")
#define CFG_KEY_VALUE_DELIMITER (':')
#define CFG_COMMENT_CHAR        ('#')

#define BOOT_ENTRY_INIT { NULL, NULL, NULL, FALSE, NULL }
#define BOOT_ENTRY_ARR_INIT { NULL, 0 }


// initial ramdisk
#define INITRD_ARG_STR ("initrd=")


