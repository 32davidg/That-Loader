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
                continue;
            }

            // Get the key-value pair in the line
            char_t* key = NULL;
            char_t* value = NULL;
            if(ParseKeyValuePair(line, CFG_KEY_VALUE_DELIMITER, &key, &value) == FALSE)
            {
                free(key);
                free(value);
                continue;
            }

            // Trim spaces before passing into AssignValueToEntry
            const char_t* trimmedkey = TrimSpaces(key);
            const char_t* trimmedvalue = TrimSpaces(value);
            if(!AssignValueToEntry(trimmedkey, trimmedvalue, &entry))
            {
                // free the value if it wasnt assigned (key stays)
                free(value);
            }
            free(key);
        }
        free(strippedEnty);

        //Fill the data, ketnel path, version and args
        if(entry.isDirectoryToKernel)
        {
            PrepareKernelDirEntry(&entry);
        }

        // Make sure the entry is valid, if it is, append it to array of entries
        if(ValidateEntry(&entry))
        {
            AppendEntry(&bootEntryArr, &entry);
        }
        else // free memory of invalid entry
        {
            FreeConfigEntry(&entry);
        }
        filePtr += ptrIncrement;
    }
    free(configData);
    if(bootEntryArr.numOfEntries ==0)
    {
        Log(LL_ERROR, 0, "The configuration file is has incorrect entries or is empty");
    }
    return bootEntryArr;
}


/*
* Check if entry is valid - holds valid inforamtion
*/
static boolean_t ValidateEntry(boot_entry_s* newEntry)
{
    // could be a block of comments, so dont print anything
    if (newEntry->name == NULL && newEntry->imageToLoad == NULL && newEntry->imageArgs == NULL)
    {
        return FALSE;
    }

    if (strlen(newEntry->name) == 0)
    {   
        if (!ignoreEntryWarnings)
        {
            Log(LL_WARNING, 0, "Ignoring unnamed entry");
        }
        return FALSE;
    }
    else if (strlen(newEntry->imageToLoad) == 0)
    {
        if (!ignoreEntryWarnings)
        {
            Log(LL_WARNING, 0, "Ignoring entry: %s, no 'path' or 'kerneldir' specified. ", newEntry->name);
        }
        return FALSE;
    }
    return TRUE;
}

/*
*   This function appends a string to the entry argument
*   Space is appended if args arent null
*/
static void AppendToArgs(boot_entry_s* entry, char_t* value)
{
    size_t argsLen = strlen(entry->imageArgs);
    size_t valueLen = strlen(value);

    // Resize args
    size_t newSize = argsLen + valueLen +1;
    entry->imageArgs = realloc(entry->imageArgs, newSize);

    // make sure to add a null char if there are no args yet
    if (argsLen == 0)
    {
        entry->imageArgs[argsLen] = CHAR_NULL;
    }
    else // add a space to seperate args
    {
        entry->imageArgs[argsLen] = ' ';
        argsLen++;
        entry->imageArgs[argsLen] = CHAR_NULL;
    }
    // append new arg
    strncpy(entry->imageArgs + argsLen, value, valueLen);
}

/*
* This function assignes char_t* values to boot_entry_s* based on const char_t* key
* FALSE as return value means the value wasnt assigned and needs to be freed
* TRUE means the value was assigned, and doesnt need to be freed
*/
static boolean_t AssignValueToEntry(const char_t* key, char_t* value, boot_entry_s* entry)
{
    //Igonre empty values
    if(value[0] == CHAR_NULL)
    {
        Log(LL_WARNING, 0, "Ignoring empty lines by key: '%s'.", key);
        return FALSE;
    }

    // kernel's name
    if(strcmp(key, "name") == 0) // if value holds the name
    {
        if(entry->name != NULL)
        {
            LogKeyRedefinition(key, entry->name, value);
            return FALSE;
        }
        // Shorten the name of it is too long
        if(strlen(value) > MAX_ENTRY_NAME_LEN)
        {
            value[MAX_ENTRY_NAME_LEN] = CHAR_NULL;
        }
        entry->name = value;
    }

    // path to kernel image
    else if (strcmp(key,"path") == 0)
    {
        if(entry->isDirectoryToKernel)
        {
            Log(LL_WARNING, 0, "'%s' and 'kerneldir' defined in the same entry. (where kerneldir=%s)",
            key, entry->kernelScanInfo->kernelDirectory);
            return FALSE;
        }
        if(entry->imageToLoad != NULL)
        {
            LogKeyRedefinition(key, entry->imageToLoad, value);
            return FALSE;
        }
        entry->imageToLoad = value;
    }

    // path to kernel directory
    else if (strcmp(key, "kerneldir") == 0)
    {
        if(entry->imageToLoad != NULL)
        {
            Log(LL_WARNING, 0, "'%s' and 'path' defined in the same entry. (where path=%s)",
            key, entry->imageToLoad);
            return FALSE;

        }
        if(entry->isDirectoryToKernel)
        {
            LogKeyRedefinition(key, entry->kernelScanInfo->kernelDirectory, value);
            return FALSE;
        }
        
        entry->kernelScanInfo = malloc(sizeof(kernel_scan_info_s));
        entry->kernelScanInfo->kernelDirectory = value;
        entry->isDirectoryToKernel = TRUE;
    }

    // add args to kernel loading args
    else if(strcmp(key, "args") == 0)
    {
        AppendToArgs(entry, value);
    }

    // This key simplifies the configuration but it just takes the value and adds it to the args
    else if (strcmp(key, "initrd") == 0)
    {
        size_t initrdLen = strlen(INITRD_ARG_STR);
        size_t valueLen = strlen(value);
        const size_t totalLen = initrdLen + valueLen + 1;

        // Create the full arg string 'initrd=<value>'
        char_t* argStr = (char_t*) malloc(totalLen * sizeof(char_t));
        if(argStr == NULL)
        {
            Log(LL_ERROR, 0, "Failed to allocate memory for buffer");
            return FALSE;
        }
        strncpy(argStr, INITRD_ARG_STR, initrdLen);
        strncpy(argStr + initrdLen, value, valueLen);
        AppendToArgs(entry, argStr);
        free(argStr);
    }
    else
    {
        boolean_t isRuntimeCfgKey = EditRuntimeConfig(key, value);
        if (!isRuntimeCfgKey)
        {
            Log(LL_WARNING, 0, "Unknown key '%s' in the config file", key);
        }
        else 
        {
            // Avoid false warnings when runtime config keys are on their own
            ignoreEntryWarnings = TRUE;
        }
        return FALSE;
    }
    return TRUE;
}

/*
* 
*/


