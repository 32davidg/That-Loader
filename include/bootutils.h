#pragma once
#include <uefi.h>
// This utility file holds the neccesary utilities for the boot proc

#define FALSE ((boolean_t)0)
#define TRUE ((boolean_t)1)

#define DEFAULT_WATCHDOG_TIMEOUT (300)


#define INPUT_TIMER_ERROR   (-1)
#define INPUT_TIMER_TIMEOUT (0)
#define INPUT_TIMER_KEY     (1)

wchar_t* StringToWideString(char_t* str);

efi_handle_t GetFileDeviceHandle(char_t* path);
efi_status_t ReadFile(efi_file_handle_t* fileHandle, uintn_t fileSize, char_t** buffer);
efi_status_t GetFileInfo(efi_file_handle_t* fileHandle, efi_file_info_t* fileInfo);
char_t* GetFileContent(char_t* path, uint64_t* outFileSize);
uint64_t GetFileSize(FILE* file);

efi_status_t RebootDevice(boolean_t rebootToFirmware);
int32_t WaitForInput(uint32_t timeout);

void EnableWatchdogTimer(uintn_t seconds);
void DisableWatchdogTimer(void);
