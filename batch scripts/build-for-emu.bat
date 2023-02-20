@echo off

rem complie .c file to .o file, and .o file to .EFI file
gcc -Wall -Werror -m64 -mabi=ms -ffreestanding -c ../src/ThatLoader.c -o ../src/ThatLoader.o
gcc -Wall -Werror -m64 -mabi=ms src/ThatLoader.o -nostdlib -shared -Wl,-dll -Wl,--subsystem,10 -e efi_main -o ../src/BOOTX64.EFI


rem compile kernel file to .o file
gcc -m64 -mabi=ms -Wl,--oformat=binary -c ..\kernel\kernel.c -e main -o ..\kernel\kernel.bin

rem delete .o file
for /f "delims=" %%f in ('dir /b ..\src\*.o') do del /f "src\%%f"

rem mount drive and move efi file to EFI/boot
OSFMount -a -t file -f ..\drive\drive.hdd -s 40M -o rw -m F:

rem copy .EFI file to EFI/Boot (our hdd image)
xcopy ..\src\BOOTX64.EFI F:\EFI\BOOT /y

rem copy .o kernel file to hdd image (F:\)
xcopy ..\kernel\kernel.bin F:\ /y

timeout 1

rem unmount drive
OSFMount -D -m F:

rem emulate file in qemu
echo Environment ready! Running emulation!
qemu-system-x86_64 -drive format=raw,unit=0,file=drive\drive.hdd -bios drive\bios64.bin -m 256M -display sdl -vga std -name TESTOS -machine q35

rem close all cmd files opened by OSFMount
taskkill /im cmd.exe /f


pause