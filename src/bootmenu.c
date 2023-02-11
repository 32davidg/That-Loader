#include "../include/bootmenu.h"
#include "../include/ErrorCodes.h"
#include "../include/efilibs.h"
#include "../include/bootutils.h"

boot_menu_cfg_s bmcfg; // boot menu config
void StartBootManager()
{

    //InitBootMenuConfig()
    // for later, when reading from config file
    while(TRUE)
    {
        
        ST->ConOut->ClearScreen(ST->ConOut);
        ST->ConOut->SetAttribute(ST->ConOut, EFI_TEXT_ATTR(EFI_LIGHTGRAY, EFI_BLACK));
        ST->ConOut->EnableCursor(ST->ConOut, FALSE);
        SetTextPosition(29, 0);
        ST->ConOut->OutputString(ST->ConOut, L"Welcome to That Loader!\r\n");

        //boot_entry_array_s bootEntries = ParseConfig();
        // load config into bootEnteries, that will be done later
        ST->ConIn->Reset(ST->ConIn, 0); // clean input buffer


        //check if Boot entries were parsed correctly
        //if (bootEntries.numOfEntries == 0)
        //{
        //    FailMenu(BAD_CONFIGURATION_ERR_MSG);
        //}
        //else
        //{
        //    BootMenu(&bootEntries);
        //}


        //clear up boot entries
        //FreeConfigEntries(&bootEntries);
        bmcfg.selectedEntryIndex = 0;
        bmcfg.entryOffset = 0;

    }
    
}

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
        
    
        ST->ConIn->Reset(ST->ConIn, 0);
        efi_status_t status = CheckKey();
        if(status == EFI_SUCCESS)
        {
            while(1)
            {
                if(GetKey('1') == 1)
                {
                    // return to main menu
                    returnToMainMenu =TRUE;
                    break;
                }
                if(GetKey('2') == 1)
                {
                    // todo open shell
                    break;
                }
                if(GetKey('3') == 1)
                {
                    //show error log
                    // todo
                    break;

                }
                if(GetKey('4') == 1)
                {
                    // warm reboot
                    ST->RuntimeServices->ResetSystem(EfiResetWarm, EFI_SUCCESS, 0, 0);
                    break;            
                }
                if(GetKey('5') == 1)
                {
                    //shutdown
                    ST->RuntimeServices->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, 0);
                    break;
                }
            }
        }
    }
    ST->ConOut->ClearScreen(ST->ConOut);

}



