@echo off

rem mount drive and move efi file to EFI/boot
OSFMount -a -t file -f drive\drive.hdd -s 40M -o rw -m F:

rem copy .EFI file to EFI/Boot (our hdd image)
xcopy src\pooploader_x64.efi F:\EFI\BOOT /y


timeout 1

rem unmount drive
OSFMount -D -m F:

rem emulate file in qemu
echo Environment ready! Running emulation!
qemu-system-x86_64 -drive format=raw,unit=0,file=drive\drive.hdd -bios drive\bios64.bin -m 256M -display sdl -vga std -name TESTOS -machine q35

rem close all cmd files opened by OSFMount
taskkill /im cmd.exe /f


pause