#include "../include/ErrorCodes.h"

void PrintCommandError(const char_t* cmd, const char_t* args, const uint8_t error)
{
    printf("%s: ", cmd);
    if (args != NULL)
    {
        printf("%s: ", args);
    }
    printf("%s\n", GetCommandErrorInfo(error));
}

