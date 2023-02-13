#pragma once
#include <uefi.h>
// This utility file holds the neccesary utilities for the boot proc

#define FALSE ((boolean_t)0)
#define TRUE ((boolean_t)1)

#define DEFAULT_WATCHDOG_TIMEOUT (300)



void EnableWatchdogTimer(uintn_t seconds);

char_t* GetFileContent(char_t* path, uint64_t* outFileSize);

