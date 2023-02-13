#pragma once
#include <uefi.h>
#include "shelldefs.h"

//edit cmd stuff
char_t* ConcatPaths(const char_t* lhs, const char_t* rhs); 
uint8_t NormalizePath(char_t** path);
void CleanPath(char_t** path);
char_t* MakeFullPath(char_t* pathArg, char_t* currPathPtr, boolean_t* isDynamicMemory);


// input
efi_input_key_t GetInputKey(void);
void GetInputString(char_t buffer[], const uint32_t maxInputSize, boolean_t hideInput);

// edit other cmd stuff
boolean_t IsPrintableChar(const char_t c);
boolean_t IsSpace(const char_t c);
char_t* TrimSpaces(char_t* str);
void RemoveRepeatedChars(char_t* str, char_t toRemove);
int32_t GetValueOffset(char_t* line, const char delimiter);

boolean_t FindFlagAndDelete(cmd_args_s** argsHead, const char_t* flagStr);
cmd_args_s* GetLastArg(const char_t* src, const char_t* dest);

int32_t PrintFileContent(char_t* path);
int32_t CreateDirectory(char_t* path);
int32_t CopyFile(const char_t* src, const char_t* dest);

char_t* StringReplace(const char_t* orig, const char_t* pattern, const char_t* replacement);

void PrintEmptyLine(void);
void PadRow(void);





