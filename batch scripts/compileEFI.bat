gcc -Wall -Werror -m64 -mabi=ms -ffreestanding -c ../src/ThatLoader.c -o ../src/ThatLoader.o
gcc -Wall -Werror -m64 -mabi=ms ../src/ThatLoader.o -nostdlib -shared -Wl,-dll -Wl,--subsystem,10 -e efi_main -o ../src/BOOTX64.EFI

for /f "delims=" %%f in ('dir /b src\*.o') do del /f "src\%%f"

gcc -m64 -mabi=ms -Wl,--oformat=binary -c ..\kernel\kernel.c -e main -o ..\kernel\kernel.bin


pause
