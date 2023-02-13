#include "../include/bootutils.h"


// Start a watchdog timer
//It works by continuously counting down from a specified time interval
// and if the software fails to reset the timer before it reaches zero
// the watchdog timer generates a system reset
void EnableWatchdogTimer(uintn_t seconds)
{
    efi_status_t status = BS->SetWatchdogTimer(seconds, 0, 0, NULL);
    if (EFI_ERROR(status))
    {
        Log(LL_WARNING, status, "Failed to set watchdog timeout to %d seconds.", seconds);
    }
}


// The function reads the file content into a dynamically allocated buffer (null terminated)
// The buffer must be freed by the user
// outFileSize is an optional parameter, it will contain the file size
char_t* GetFileContent(char_t* path, uint64_t* outFileSize)
{
    char_t* buffer = NULL;
    FILE* file = fopen(path, "r");
    if (file != NULL)
    {
        // Get file size
        uint64_t fileSize = GetFileSize(file);
        if (outFileSize != NULL)
        {
            *outFileSize = fileSize;
        }

        buffer = malloc(fileSize + 1);
        if (buffer != NULL)
        {
            fread(buffer, 1, fileSize, file);
            buffer[fileSize] = CHAR_NULL;
        }
        else
        {
            Log(LL_ERROR, 0, "Failed to create buffer to read file.");
        }
        fclose(file);
    }
    return buffer;
}