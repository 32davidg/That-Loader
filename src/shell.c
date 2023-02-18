#include "../include/shell.h"
#include "../include/commands.h"
#include "../include/shellutils.h"
#include "../include/bootutils.h"
#include "../include/logs.h"
#include "../include/ErrorCodes.h"


#define SHELL_MAX_INPUT (128)

#define SPACE (' ') // Used as a delimiter between a command and the arguments
#define QUOTATION_MARK ('"')

#define SHELL_EXIT_STR ("exit")


// shell functions
static int8_t ShellLoop(char_t** currPathPtr);
static int8_t ProcessCommand(char_t buffer[], char_t** currPathPtr);

// Command and arguments processing
static char_t* GetCommandFromBuffer(char_t buffer[]);
static int8_t ParseArgs(char_t* inputArgs, cmd_args_s** outputArgs);
static int8_t SplitArgsString(char_t buffer[], cmd_args_s** outputArgs);
static cmd_args_s* InitializeArgsNode(void);
static void AppendArgsNode(cmd_args_s* head, cmd_args_s* node);
static void FreeArgs(cmd_args_s* args);



/*
* This function initalizes the shell.
* sets the default path as root (\).
* Dynamically allocates the path for further changes
*/
int8_t StartShell(void)
{
    Log(LL_INFO, 0, "Starting shell...");
    ST->ConOut->ClearScreen(ST->ConOut);
    ST->ConOut->EnableCursor(ST->ConOut, TRUE);

    printf("Welcome to the shittiest shell!\n"
    "Type 'help' to get a list of commands.\n"
    "Type 'help cmd' command info.\n");
    

    // 2 is the initial size for the root directory "\" and null string terminator
    char_t* currPath = malloc (2*sizeof(char_t));
    if(currPath == NULL)
    {
        Log(LL_ERROR, 0, "Failed to allocate memory for path. (duing shell initialization)");
        return CMD_OUT_OF_MEMORY;
    }
    // Initialize the default starting path
    currPath[0] = '\\';
    currPath[1] = CHAR_NULL;

    ST->ConIn->Reset(ST->ConIn, 0);

    if(ShellLoop(&currPath) == 1)
    {
        return CMD_OUT_OF_MEMORY;
    }

    // cleanup
    Log(LL_INFO, 0, "Closing shell.");
    free(currPath);
    ST->ConOut->EnableCursor(ST->ConOut, FALSE);
    ST->ConOut->ClearScreen(ST->ConOut);
    return 0;

}


/*
* Run an infinate loop, reading command to buffer
* pass command to ProcessCommand func
*/
static int8_t ShellLoop(char_t** currPathPtr)
{
    while (TRUE)
    {
        char_t* buffer[SHELL_MAX_INPUT] = {0};
        printf("> ");

        GetInputString(buffer, SHELL_MAX_INPUT, FALSE);

        if(strcmp(buffer, SHELL_EXIT_STR) == 0)
        {
            break;
        }
        if(ProcessCommand(buffer, currPathPtr) == 1)
        {
            return CMD_OUT_OF_MEMORY;
        }
    
    }
    return CMD_SUCCESS;
}

static int8_t ProcessCommand(char_t buffer[], char_t** currPathPtr)
{
    buffer = TrimSpaces(buffer);

    char_t* args = buffer;
    char_t* cmd = GetCommandFromBuffer(buffer);

    if( cmd == NULL)
    {
        return 0;
    }

    // Parse the arguments into a linked list (if there are any)
    cmd_args_s* cmdArgs = NULL;
    if(args != NULL)
    {
        int8_t res = ParseArgs(args, &cmdArgs);
        if(res != CMD_SUCCESS)
        {
            PrintCommandError(cmd, args, res);
            return res;
        }
    }

    const uint8_t* allCmds = CommandCount();
    for (uint8_t i = 0; i < allCmds; i++)
    {
        // Find the right command and execute the command function
        if(strcmp(cmd, commands[i].commandName) == 0)
        {
            // Pass a pointer to the head of the linked list since it might be modified
            commands[i].CommandFunction(&cmdArgs, currPathPtr);
            break;
        }
        else if (i+1 == allCmds)
        {
            printf("Command '%s' not found\n", commands[i].commandName);

        }
    }

    free(cmd);
    FreeArgs(cmdArgs);
    return CMD_SUCCESS;
}


/*
* Get the actual command keyword from a buffer (and return it)
*/
static char_t* GetCommandFromBuffer(char_t* buffer[])
{
    size_t bufferLen = strlen(buffer);
    if(bufferLen == 0)
    {
        return NULL;
    }

    int32_t cmdLen = 0;
    int32_t cmdoffset = GetValueOffset(buffer, ' ');
    if(cmdoffset == -1)
    {
        cmdLen = bufferLen +1;
    }
    else
    {
        cmdLen = cmdoffset;
    }

    char_t* cmd = malloc(cmdLen);
    strncpy(cmd, buffer, cmdLen -1);

    return cmd;
}

/*
* Parses the args using an index.
* god knows what i did here
*/
static int8_t ParseArgs(char_t* inputArgs, cmd_args_s** outputArgs)
{
    if(inputArgs == NULL)
    {
        return CMD_SUCCESS;
    }

    const size_t argsLen = strlen(inputArgs);
    char_t tempBuffer[SHELL_MAX_INPUT] = {0};
    uint8_t bufferIdx = 0; // ccurrent index of the buffer where the next char is stoerd

    boolean_t quotationMarkOpened = FALSE;
    for(size_t i = 0; i <= argsLen; i++)
    {
        if (inputArgs[i] == QUOTATION_MARK)
        {
            // if the quotation mark is the last char in the arg
            if (quotationMarkOpened && (inputArgs[i+1] == SPACE || inputArgs[i+1] == CHAR_NULL))
            {
                int8_t res = SplitArgsString(tempBuffer, outputArgs);
                if (res != CMD_SUCCESS)
                {
                    return res;
                }
                quotationMarkOpened = FALSE;
            }
            // If the quotation mark is the first char in the argument
            else if( i==0 || inputArgs[i-1] == SPACE)
            {
                quotationMarkOpened = TRUE;
            }
            else // add the quotation mark if its in the middle of the arg string
            {
                //goto addChar;
            }
        }
        // Split args
        else if (inputArgs[i] == SPACE)
        {
            //if the quotation mark are opened, and a space char is hit, well keep it
            if(quotationMarkOpened)
            {
                //goto addChar;
            }
            else{
                int8_t res = SplitArgsString(tempBuffer, outputArgs);
                if(res != CMD_SUCCESS)
                {
                    return res;
                }
                bufferIdx = 0; // Reset buffer index
            }

        }
        else if(inputArgs[i] == CHAR_NULL)
        {
            // quotation mark wasnt closed
            if(quotationMarkOpened)
            {
                return CMD_QUOTATION_MARK_OPEN;
            }
            else
            {
                int8_t res = SplitArgsString(tempBuffer, outputArgs);
                if (res != CMD_SUCCESS)
                {
                    return res;
                }
            }
        }
        else{
            addChar:
                tempBuffer[bufferIdx] = inputArgs[i];
                bufferIdx++;
        }
    }
    return CMD_SUCCESS;
}







