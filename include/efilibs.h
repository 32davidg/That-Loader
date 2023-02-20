#include <uefi.h>
#include "graphics.h"

#define DEFAULT_CONSOLE_COLUMNS (80)
#define DEFAULT_CONSOLE_ROWS    (30);

// Sets the Column and Row of the text screen cursor position.
void SetTextPosition(uint32_t Col, uint32_t Row)
{
    ST->ConOut->SetCursorPosition(ST->ConOut, Col, Row);
}

// This resets the whole console ( A.K.A. your display screen ) interface.
void ResetScreen()
{
    ST->ConOut->Reset(ST->ConOut, 1);
}

// This clears the screen buffer, but does not reset it.
void ClearScreen()
{
    ST->ConOut->ClearScreen(ST->ConOut);
}



void HitAnyKey()
{
    // clears the keyboard buffer
    ST->ConIn->Reset(ST->ConIn, 1);

    // setup the struct to take keyboard input
    efi_input_key_t Key;

    // wait in while loop for keystroke 
    while((ST->ConIn->ReadKeyStroke(ST->ConIn, &Key)) == EFI_NOT_READY);
}

// reset keyboard buffer
void ResetKeyboard()
{
    ST->ConIn->Reset(ST->ConIn, 1);
}

// checks if CHAR16 key and CheckKeystroke have the same value
// returns true if both match, otherwise returns false
efi_input_key_t CheckKeystroke;
boolean_t GetKey(char_t key)
{
    if(CheckKeystroke.UnicodeChar == key)
    {
        return 1;
    } 
    else 
    {
        return 0;
    }
}

// check if there is a keystroke saved
// if it does exist return EFI_SUCCESS
// and it stores in in CheckKeystroke
efi_status_t CheckKey()
{
    return ST->ConIn->ReadKeyStroke(ST->ConIn, &CheckKeystroke);
}
