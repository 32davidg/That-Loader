#include "../include/logs.h"
//#include "../include/shellutils.h"

#define LOG_PATH ("\\EFI\\thatloader\\log.txt")
#define OLD_LOG_PATH ("\\EFI\\thatloader\\log.txt.old")

#define SECONDS_IN_DAY (86400)
#define SECONDS_IN_HOUR (3600)
#define SECONDS_IN_MINUTE (60)

static efi_time_t timeSinceInit = {0};

// Init log file
// create an empty logfile, and init timeSinceInit var
// return 1 on success, 0 on failure
int8_t InitLogger(void){
    // if a logfile already exists, save it as old log
    FILE* fp = fopen(LOG_PATH, "r");
    if (fp != NULL)
    {
        fclose(fp);
        // copy contents of this log file to another
    }
}