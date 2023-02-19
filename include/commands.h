#pragma once
#include <uefi.h>
#include "shelldefs.h"

boolean_t HelpCmd(cmd_args_s** args, char_t** currPathPtr);
const char_t* HelpBrief(void);
const char_t* HelpLong(void);

uint8_t CommandCount(void);

extern const shell_cmd_s commands[];