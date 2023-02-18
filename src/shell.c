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