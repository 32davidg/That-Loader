#include "../include/configfile.h"
#include "../include/logs.h"
#include "../include/bootutils.h"
#include "../include/shellutils.h"
#include "../include/bootmenu.h"
#include "../include/ErrorCodes.h"

// config file path
#define CFG_PATH ("\\EFI\\thatloader\\config.cfg")

#define MAX_ENTRY_NAME_LEN (70)

#define LINUX_KERNEL_IDENTIFIER_STR ("vmlinuz")
#define STR_TO_SUBSTITUTE_WITH_VERSION ("%v")

#define CFG_LINE_DELIMITER      ("\n")
#define CFG_ENTRY_DELIMITER     ("\n\n")
#define CFG_KEY_VALUE_DELIMITER (':')
#define CFG_COMMENT_CHAR        ('#')

#define BOOT_ENTRY_INIT { NULL, NULL, NULL, FALSE, NULL }
#define BOOT_ENTRY_ARR_INIT { NULL, 0 }


// initial ramdisk
#define INITRD_ARG_STR ("initrd=")


/* Basic config parser functions */
static boolean_t AssignValueToEntry(const char_t* key, char_t* value, boot_entry_s* entry);
static boolean_t ValidateEntry(boot_entry_s* newEntry);
static void AppendEntry(boot_entry_array_s* bootEntryArr, boot_entry_s* entry);
static boolean_t EditRuntimeConfig(const char_t* key, char_t* value);

/* Functions related to the "kerneldir" key in the config */
static void PrepareKernelDirEntry(boot_entry_s* entry);
static char_t* GetPathToKernel(const char_t* directoryPath);
static char_t* GetKernelVersionString(const char_t* fullKernelFileName);

static void FreeConfigEntry(boot_entry_s* entry);
static inline void LogKeyRedefinition(const char_t* key, const char_t* curr, const char_t* ignored);

static void AppendToArgs(boot_entry_s* entry, char_t* value);

static boolean_t ignoreEntryWarnings;

// This Function returns a pointer to the head of the linked list of the boot entries
// Every Pointer in the linked list was allocated dynamically
boot_entry_array_s ParseConfig(void)
{
    Log(LL_INFO, 0, "Parsing config file...");

    boot_entry_array_s bootEntryArr = BOOT_ENTRY_ARR_INIT;
    
    uint64_t fileSize = 0;
    char_t* configData = GetFileContent(CFG_PATH, &fileSize);
    if(configData == NULL)
    {
        Log(LL_ERROR, 0, "Failed to read configuration file");
        return bootEntryArr;
    }
    
    // Tracks the pointer in the file
    char_t* filePtr = configData;


    // Get blocks of entries in a loop - one kernel config (boot_entry_s) at a time
    // Once (filePtr >= configData +filesize) it means we finished reading the file contents
    while (filePtr < configData + fileSize)
    {
        boot_entry_s entry = BOOT_ENTRY_INIT;
        ignoreEntryWarnings = FALSE;

        // Get a pointer to the end of an entry 
        char_t* configEntryEnd = strstr(filePtr, CFG_ENTRY_DELIMITER);

        size_t entryStrLen = 0;
        size_t ptrIncrement = 0; // Used to increment filePtr

        if(configEntryEnd == NULL) // if at last entry
        {
            entryStrLen = strlen(filePtr);
            ptrIncrement = entryStrLen;
        }
        else 
        {
            entryStrLen = configEntryEnd - filePtr;
            ptrIncrement = entryStrLen + strLen(CFG_ENTRY_DELIMITER);
        }


        // Increment file pointer and skip empty lines
        if (entryStrLen ==0)
        {
            filePtr += ptrIncrement;
            continue;
        }

        // Holds only the current entry block
        char_t* strippedEnty = malloc(entryStrLen+ 1);
        if(strippedEnty == NULL)
        {
            Log(LL_ERROR, 0, "Failed to allocate memory for entry block");
            free(configData);
            return bootEntryArr;
        }
        strcpy(strippedEnty, filePtr, entryStrLen); // copy the entry to strippedEnty

        // Create a copy because we need to keep the original pointer to free it
        // and strtok modifies the pointer
        char_t* entryCopy = strippedEnty;
        char_t* line = NULL;
        // Gets lines from entry blocks and parses them
        while ((line = strtok_r(entryCopy, CFG_LINE_DELIMITER, &entryCopy)) == NULL)
        {
            // Ignore comments
            if (line[0] == CFG_COMMENT_CHAR)
            {
                
            }
        }



    }
}


