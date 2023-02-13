#include "../include/shellutils.h"
#include "../include/logs.h"
#include "../include/bootutils.h"
#include "../include/ErrorCodes.h"
#include "../include/display.h"

#define DIRECTORY_DELIM ('\\')
#define DIRECTORY_DELIM_STR ("\\")
#define CURRENT_DIR (".")
#define PREVIOUS_DIR ("..")


/*
* This function concatenates two path sides
* lhs = "/usr/bin" + rhs = "/lib/lol" = /usr/bin/lib/lol
* lhs - left hand side, rhs - right hand side
* func caller has to free the char_t* buffer
*/
char_t* ConcatPaths(const char_t* lhs, const char_t* rhs)
{
    size_t lhsLen = strlen(lhs);
    size_t rhsLen = strlen(rhs);

    // null terminator + additinal '/' between the rhs and lhs
    char_t* newPath = malloc(lhsLen + rhsLen + 2);
    if(newPath == NULL)
    {
        Log(LL_ERROR, 0, "Failed to allocate memory for two path concatenation...");
        return NULL;
    }
    strncpy(newPath, lhs, lhsLen); // copy the left path to newly allocated bufer

    //before adding the right hand side, we have to make sure that only one side has has '/'
    // if the last char in lhs is not '/' and the first char in rhs is not '/', we have to append it
    size_t lhsLastIndex = strlen(lhs) -1;
    if(lhs[lhsLastIndex] != '\\' && rhs[0] != '\\')
    {
        strcpy(newPath + lhsLen, '\\', 1);
    }
    // Avoid duplicating '\\' in newPath
    if(lhs[lhsLastIndex] == rhs[0] == '\\')
    {
        rhs++; // advance pointer to next character
    }
    strncat(newPath + lhsLen, rhs, rhsLen);

    return newPath; // note the newPath wasnt freed, the once calling the function has to free it
}
// remove the "." and the ".." dirs from the given path
// shoutout chatgpt for this
uint8_t NormalizePath(char_t** path)
{
     // count the amount of tokens
    char_t* copy = *path;
    uint16_t tokenAmount = 0;
    while (*copy != CHAR_NULL)
    {
        if (*copy == DIRECTORY_DELIM)
        {
            tokenAmount++;
        }
        copy++;
    }
    
    // Nothing to normalize
    if (tokenAmount <= 1) 
    {
        return CMD_SUCCESS;
    }

    char_t** tokens = malloc(tokenAmount * sizeof(char_t*));
    if (tokens == NULL)
    {
        Log(LL_ERROR, 0, "Failed to allocate memory while normalizing the path.");
        return CMD_OUT_OF_MEMORY;
    }
    tokens[0] = NULL;
    char_t* token = NULL;
    char_t* srcCopy = *path + 1; // Pass the first character (which is always "\")
    uint16_t i = 0;
    // Evaluate the path
    while ((token = strtok_r(srcCopy, DIRECTORY_DELIM_STR, &srcCopy)) != NULL)
    {
        // Ignore the "." directory
        if (strcmp(token, CURRENT_DIR) == 0)
        {
            tokenAmount--;
        }
        // Go backwards in the path
        else if (strcmp(token, PREVIOUS_DIR) == 0)
        {
            if (tokenAmount > 0) 
            {
                tokenAmount--;
            }
            else
            {
                tokenAmount = 0;
            }

            // Don't go backwards past the beginning
            if (i > 0) 
            {
                i--;
            }

            if (tokens[i] != NULL)
            {
                if (tokenAmount > 0) 
                {
                    tokenAmount--;
                }
                free(tokens[i]);
                tokens[i] = NULL;
            }
        }
        else
        {
            tokens[i] = strdup(token);
            i++;
        }
    }
    // Rebuild the string
    (*path)[0] = '\\';
    (*path)[1] = CHAR_NULL;
    for (i = 0; i < tokenAmount; i++)
    {
        strcat(*path, tokens[i]);

        if (i + 1 != tokenAmount)
        {
            strcat(*path, "\\");
        }
            
        free(tokens[i]);
    }
    free(tokens);
    return CMD_SUCCESS;
}

// make path usable and clean
// get rid of unnecessary backslashes, and other whitespace chars
void CleanPath(char_t** path)
{
    path = TrimSpaces(*path);

    //remove duplicate backslashes from the command
    RemoveRepeatedChars(*path, DIRECTORY_DELIM);

    // Remove a backslash from the end if it exists
    size_t lastIndex = strlen(*path) - 1;
    if ((*path)[lastIndex] == DIRECTORY_DELIM && lastIndex + 1 > 1) 
    {
        (*path)[lastIndex] = 0;
    }

}

/*
* function that gets an input path, and inputs argument, and returns the full path to pathArg object
*/
char_t* MakeFullPath(char_t* pathArg, char_t* currPathPtr, boolean_t* isDynamicMemory)
{
    char_t* fullPath = NULL;

    CleanPath(&pathArg);

    //check if path starts from the root dir
    if (pathArg[0] == DIRECTORY_DELIM)
    {
        fullPath = pathArg;
        *isDynamicMemory  = FALSE;
    }
    // if the args are only whitespace
    else if (pathArg[0] == CHAR_NULL)
    {
        *isDynamicMemory = FALSE;
        return NULL;
    }
    else // Check the concatenated path
    {
        fullPath = ConcatPaths(currPathPtr, pathArg);
        if (fullPath == NULL)
        {
            return NULL;
        }
        *isDynamicMemory = TRUE;
    }
    return fullPath;
}


// get an inputted key
efi_input_key_t GetInputKey(void)
{
    efi_status_t status = 0;
    efi_input_key_t key = {0};

    do
    {
        uintn_t index;
        BS->WaitForEvent(1, &ST->ConIn->WaitForKey, &index);
        status = ST->ConIn->ReadKeyStroke(ST->ConIn, &key);
    } while (EFI_ERROR(status));
    EnableWatchdogTimer(DEFAULT_WATCHDOG_TIMEOUT);
    return key;
}


// check if given char is a printable value (ascii)
boolean_t IsPrintableChar(const char_t c)
{
    return (c  >= ' ' && c <= '~');
}

// get an inputed string
void GetInputString(char_t buffer[], const uint32_t maxInputSize, boolean_t hideInput)
{
    uint32_t index = 0;
    
    while(TRUE)
    {
        // read input continuously 
        efi_input_key_t key = GetInputKey();
        char_t unicodechar = key.UnicodeChar;

        // procsses input, enter is \n
        if (unicodechar == CHAR_CARRIAGE_RETURN) 
        {
            putchar('\n');
            break;
        }

        // backspace (delete last char)
        if (unicodechar == CHAR_BACKSPACE)
        {
            if (index > 0) // Dont delete when the buffer is empty
            {
                index--;
                buffer[index] = 0;
                printf("\b \b"); // Destructive backspace
            }
        }
        // Add the character to the buffer as long as there is enough space and if its a valid character
        // The character in the last index must be null to terminate the string
        else if (index < maxInputSize - 1 && IsPrintableChar(unicodechar))
        {
            buffer[index] = unicodechar;
            index++;

            if (hideInput) // When entering a password
            {
                putchar('*');
            }
            else
            {
                putchar(unicodechar);
            }
        }
       
    }
}

// check if a char is a whitespace character
boolean_t IsSpace(const char_t c)
{
    return (c == ' ' || c == CHAR_TAB);
}


/*
* Trim accidental spaces form input
*/
char_t* TrimSpaces(char_t* str)
{
    size_t stringLen = strlen(str);
    char_t* originalString = str;

    // remove trailing spaces (end of the string)
    char_t* end = originalString + stringLen - 1;
    while(end > originalString && IsSpace(*end)) // check if if end of string is space
    {
        end--; // if it is, then go one char back
    }

    // remove leading whitespace (in the start of the string)
    while (IsSpace(*str))
    {
        str++;
    }

    return str;
}

void RemoveRepeatedChars(char_t* str, char_t toRemove)
{
    char_t* dest= str;
    while(*str != CHAR_NULL)
    {
        while(*str == toRemove && *(str+1) == toRemove)
        {
            str++;
        }
        *dest++ = *str++;
    }
    *dest = 0;
}



