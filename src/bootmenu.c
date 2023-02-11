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