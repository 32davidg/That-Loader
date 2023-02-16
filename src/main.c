#include "include\bootmenu.h"


int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    while (1)
    {
        ST->ConOut->ClearScreen(ST->ConOut);
        printf("Hello World!\n");

    }
    return 0;
}

