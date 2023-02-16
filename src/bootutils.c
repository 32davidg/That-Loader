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


/*
* This Function recieves a file handle a file info handle,
* write file handle, its GUID, and its size
*/
efi_status_t GetFileInfo(efi_file_handle_t* fileHandle, efi_file_info_t* fileInfo)
{
    efi_guid_t infGuid = EFI_FILE_INFO_GUID;
    uintn_t size = sizeof(efi_file_info_t);
    return fileHandle->GetInfo(fileHandle, &infGuid, &size, (void*)fileInfo);



}


// Returns the size of the file in bytes
uint64_t GetFileSize(FILE* file)
{
    if (file == NULL)
    {
        return 0;
    }
    efi_file_info_t info;
    efi_status_t status = GetFileInfo(file, &info);
    if (EFI_ERROR(status)){
        Log(LL_ERROR, status, "Failed to get file info when getting file size.");
        return 0;
    }
    return info.FileSize;

}