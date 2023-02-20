#pragma once
#include <uefi.h>

// Used in the input processing function
#define UP_ARROW_SCANCODE       (0x01)
#define DOWN_ARROW_SCANCODE     (0x02)
#define RIGHT_ARROW_SCANCODE    (0x03)
#define LEFT_ARROW_SCANCODE     (0x04)

#define PAGEUP_KEY_SCANCODE     (0x09)
#define PAGEDOWN_KEY_SCANCODE   (0x0A)

#define HOME_KEY_SCANCODE       (0x05)
#define END_KEY_SCANCODE        (0x06)

#define DELETE_KEY_SCANCODE     (0x08)

#define ESCAPE_KEY_SCANCODE     (0x17)

#define F1_KEY_SCANCODE         (0x0B)
#define F2_KEY_SCANCODE         (0x0C)

// the max rows and columns the screen can have
extern uintn_t screenRows;
extern uintn_t screenCols;

// A flag that when set to TRUE means that the console size is known and safe to use
extern boolean_t screenModeSet;

boolean_t SetMaxConsoleSize(void);
boolean_t QueryCurrentConsoleSize(void);