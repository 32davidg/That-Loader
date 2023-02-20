#include <uefi.h> 
#define DEFAULT_CONSOLE_COLUMNS (80)
#define DEFAULT_CONSOLE_ROWS    (30);

void print_border(int MAX_ROWS, int MAX_COLS) {
    for (int row = 0; row < MAX_ROWS; row++) {
        for (int col = 0; col < MAX_COLS; col++) {
            if (row == 0 || row == MAX_ROWS-1 || col == 0 || col == MAX_COLS-1) {
                ST->ConOut->SetAttribute(ST->ConOut, EFI_TEXT_ATTR(EFI_WHITE, EFI_WHITE)); // higlight text
                printf("*");
                ST->ConOut->SetAttribute(ST->ConOut, EFI_TEXT_ATTR(EFI_LIGHTGRAY, EFI_BLACK)); // go back to normal
            }
            else {
                ST->ConOut->SetAttribute(ST->ConOut, EFI_TEXT_ATTR(EFI_WHITE, EFI_WHITE)); // higlight text
                printf("*");
                ST->ConOut->SetAttribute(ST->ConOut, EFI_TEXT_ATTR(EFI_LIGHTGRAY, EFI_BLACK)); // go back to normal
            }
        }
        printf("\n");
    }
}

