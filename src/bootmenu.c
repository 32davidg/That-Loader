#include "../include/bootmenu.h"
#include "../include/shellutils.h"
#include "../include/configfile.h"
#include "../include/logs.h"
#include "../include/bootutils.h"
#include "../include/display.h"
#include "../include/shell.h"

#define F5_KEY_SCANCODE (0x0F) // Used to refresh the menu (reparse config)

#define SHELL_CHAR  ('c')
#define INFO_CHAR   ('i')

#define BAD_CONFIGURATION_ERR_MSG ("An error has occurred while parsing the config file.")
#define FAILED_BOOT_ERR_MSG ("An error has occurred during the booting process.")




// Temp forward declarations
static void InitBootMenuOutput(void);
static void FailMenu(const char_t* errorMsg);
void StartBootManager();



boot_menu_cfg_s bmcfg; // boot menu config
void StartBootManager()
{

    InitBootMenuOutput();
    while(TRUE)
    {
        
        ST->ConOut->ClearScreen(ST->ConOut);
        ST->ConOut->SetAttribute(ST->ConOut, EFI_TEXT_ATTR(EFI_LIGHTGRAY, EFI_BLACK));
        ST->ConOut->EnableCursor(ST->ConOut, FALSE);
        SetTextPosition(29, 0);
        ST->ConOut->OutputString(ST->ConOut, L"Welcome to That Loader!\r\n");


        // Config parsing is in the loop because i want the config to be updatable even when the program is running 
        boot_entry_array_s bootEntries = ParseConfig();
        


        ST->ConIn->Reset(ST->ConIn, 0); // clean input buffer


        //check if Boot entries were parsed correctly
        if (bootEntries.numOfEntries == 0)
        {
            FailMenu(BAD_CONFIGURATION_ERR_MSG);
        }
        else
        {
            BootMenu(&bootEntries);
            
        }


        //clear up boot entries
        FreeConfigEntries(&bootEntries);
        bmcfg.selectedEntryIndex = 0;
        bmcfg.entryOffset = 0;

    }
    
}
/*
* Fail menu - use it whenever somethng goes wrong
* user gets a few options to recover failed attmept
* (return to main menu, open shell, show error log file, reboot and shutdown)
*/
static void FailMenu(const char_t* errorMsg)
{
    boolean_t returnToMainMenu = FALSE;
    bmcfg.timeoutCancelled = TRUE;

    while(!returnToMainMenu)
    {
        ClearScreen();
        SetTextPosition(0, 0);
        ST->ConOut->OutputString(ST->ConOut, L"That-Loader\r\n");
        printf("Something went wrong\r\n");
        printf("%s\n\n", errorMsg);
           printf("1) Return to Main Menu\n"
           "2) Open shell\r\n"
           "3) Show error log file\r\n"
           "4) Warm reboot (Restart)\r\n"
           "5) Shutdown\r\n"
           "Choose desired option to continue\r\n");
        
        while(1)
        {
            ST->ConIn->Reset(ST->ConIn, 0);
            efi_status_t status = CheckKey();
            if(status == EFI_SUCCESS)
            {
                if(GetKey('1') == 1)
                {
                    // return to main menu
                    returnToMainMenu =TRUE;
                    break;
                }
                if(GetKey('2') == 1)
                {
                    StartShell();
                    break;
                }
                if(GetKey('3') == 1)
                {
                    ShowLogFile();
                    break;

                }
                if(GetKey('4') == 1)
                {
                    // warm reboot
                    ClearScreen();
                    Log(LL_INFO, 0, "Warm resetting machine...");
                    efi_status_t status = ST->RuntimeServices->ResetSystem(EfiResetWarm, EFI_SUCCESS, 0, 0);
                    Log(LL_ERROR, status, "Failed to reboot machine");
                    break;            
                }
                if(GetKey('5') == 1)
                {
                    ClearScreen();
                    Log(LL_INFO, 0, "Shutting down machine...");
                    //shutdown
                    ST->RuntimeServices->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, 0);
                    Log(LL_ERROR, status, "Failed to reboot machine");

                    break;
                }
            }
        }
    }
    ST->ConOut->ClearScreen(ST->ConOut);

}

/*
* Print the recent log file
*/
void ShowLogFile(void)
{
    ST->ConOut->ClearScreen(ST->ConOut);

    PrintLogFile();

    printf("\nPress any key to return...");
    GetInputKey();
}


/*
* This function sets up the screen for boot entries menu
* uses the bmcfg (boot menu config) struct to initialize the boot screen
* inits rows, columns, and all bcmfg attributes
*/
static void InitBootMenuOutput(void)
{
    // reserved rows for other entries
    const int32_t reserved_rows = 10;
    if(screenModeSet) // if screen size is set and ready to use
    {
        bmcfg.maxEntriesOnScreen = screenRows - reserved_rows;
    }
    else
    {
        bmcfg.maxEntriesOnScreen = DEFAULT_CONSOLE_ROWS - reserved_rows;
    }
    bmcfg.entryOffset = 0;
    bmcfg.timeoutSeconds = DEFAULT_TIMEOUT_SECONDS;
    bmcfg.selectedEntryIndex = 0;
    bmcfg.timeoutCancelled = FALSE;
    bmcfg.bootImmediately = FALSE;

}

/*
* change the selected index (scrool the entries)
*/
static void scrollEntries(void)
{
    // 0
    // 1
    // 2
    // 3
    if(bmcfg.selectedEntryIndex < bmcfg.entryOffset) // scroll up
    {
        bmcfg.entryOffset = bmcfg.selectedEntryIndex;
    }
    if (bmcfg.selectedEntryIndex >= bmcfg.entryOffset + bmcfg.maxEntriesOnScreen) // scroll down (bmcfg.maxEntriesOnScreen to avoid scrolling out of bounds)
    {
        bmcfg.entryOffset = bmcfg.selectedEntryIndex - bmcfg.maxEntriesOnScreen + 1;
    }
}


/*
*   This function prints the current menu entries, and highlights the selcted one
*/
static void PrintMenuEntries(boot_entry_array_s* entryArr)
{
    int32_t index = bmcfg.entryOffset;

    // Print hidden entries
    if (index > 0)
    {
        printf(" . . . %d more", index);
    }
    else{
        PrintEmptyLine();
    }
    for(int32_t i =0; i < bmcfg.maxEntriesOnScreen; i++)
    {
        //prevent goind out of bounds
        if(index >= entryArr->numOfEntries)
        {
            break;
        }

        int32_t entryNum = index + 1;
        char_t* entryName = entryArr->entryArray[index].name;
        if(index == bmcfg.selectedEntryIndex) // highlight entry
        {
            ST->ConOut->SetAttribute(ST->ConOut, EFI_TEXT_ATTR(EFI_BLACK, EFI_LIGHTGRAY));
            printf(" %d) %s", entryNum, entryName);
            ST->ConOut->SetAttribute(ST->ConOut, EFI_TEXT_ATTR(EFI_LIGHTGRAY, EFI_BLACK));
        }
        else // print normally
        {
            printf(" %d) %s", entryNum, entryName);
        }
        PadRow();

        index++;

    }

    // Print how many hidden entries are at the bottom of the screen
    if(index < entryArr->numOfEntries)
    {
        printf(" . . . . %d more", entryArr->numOfEntries);
        PadRow();
    }
    else
    {
        PrintEmptyLine();
    }


}



