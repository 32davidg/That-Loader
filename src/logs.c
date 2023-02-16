#include "../include/logs.h"
#include "../include/shellutils.h"

#define THAT_LOADER_NAME_STR "ThatLoader"

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
        CopyFile(LOG_PATH, OLD_LOG_PATH);
    }
    fp = fopen(LOG_PATH, "w");
    if (fp != NULL)
    {
        fclose(fp);
        RT->GetTime(&timeSinceInit, NULL);

        // Print the date of the log and boot maganger's name
        Log(LL_INFO, 0, "Starting %s", THAT_LOADER_NAME_STR);
        Log(LL_INFO, 0, "Log date: %02d/%02d/%04d %02d:%02d:%02d",
        timeSinceInit.Day, timeSinceInit.Month, timeSinceInit.Year, 
        timeSinceInit.Hour, timeSinceInit.Minute, timeSinceInit.Second);

        return 1;
    }

    return 0;
}


/*

*   Log (write logs) to log file  
    va_args (the ...) are for string formatting
    status parameter is optinal and can be set to 0
*/
void Log(log_level_t loglevel, efi_status_t status, const char_t* fmtMessage, ...)
{
    FILE* log = fopen(LOG_PATH, "a"); // append to file
    // Dont write to file if the logger has not been initialized or is unaccessible
    if( log == NULL || timeSinceInit.Day == 0)
    {
        return;
    }
    fprintf(log, "[%04ds] [%s] ", GetSecondsSinceInit(), LogLevelString(loglevel));
    

    // print the string and add formatting (if there is any)
    va_list args;
    va_start(args, fmtMessage);
    vprintf(log, fmtMessage, args);
    va_end(args);

    // Append UEFI error message if the status argument is an error status
    if (EFI_ERROR(status))
    {
        fprintf(log, " (EFI Error: %s)\n", EfiErrorString(status));
    }
    fclose(log);
    
}

// literally just get the seconds since the initation
time_t GetSecondsSinceInit(void)
{
    efi_time_t currTime = {0};
    efi_status_t status = RT->GetTime(&currTime, NULL);
    if(EFI_ERROR(status))
    {
        return 0;
    }
    time_t seconds = 0;
    seconds += (currTime.Day - timeSinceInit.Day) * SECONDS_IN_DAY;
    seconds += (currTime.Hour - timeSinceInit.Hour) * SECONDS_IN_HOUR;
    seconds += (currTime.Minute - timeSinceInit.Minute) * SECONDS_IN_MINUTE;
    seconds += currTime.Second - timeSinceInit.Second;   
    return seconds;
}

// Print the log file
void PrintLogFile(void)
{
    uint8_t res = PrintFileContents();
    if (res != 0)
    {
        printf("Failed to open log file\n");
    }
}

