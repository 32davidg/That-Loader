#pragma once
#include <uefi.h>

// the max rows and columns the screen can have
extern uintn_t screenRows;
extern uintn_t screenCols;

// A flag that when set to TRUE means that the console size is known and safe to use
extern boolean_t screenModeSet;

boolean_t SetMaxConsoleSize(void);
boolean_t QueryCurrentConsoleSize(void);