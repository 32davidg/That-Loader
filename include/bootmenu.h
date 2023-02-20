#pragma once
#include <uefi.h>

// while there is no actual defualt, I just eyeballed it
#define DEFAULT_CONSOLE_COLUMNS (80)
#define DEFAULT_CONSOLE_ROWS    (30)

#define DEFAULT_TIMEOUT_SECONDS (10);

typedef struct boot_menu_cfg_s
{
    int32_t maxEntriesOnScreen;
    int32_t selectedEntryIndex; // the desired entry
    int32_t entryOffset; // the current entry the program is pointing at

    int32_t timeoutSeconds;
    boolean_t timeoutCancelled;
    boolean_t bootImmediately;
} boot_menu_cfg_s;


extern boot_menu_cfg_s bmcfg;

void StartBootManager(void);

void ShowLogFile(void);






