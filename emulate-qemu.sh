#!/bin/bash
# This script is used for quick emulating and testing

# Create dirs
[ -d add-to-image ] || mkdir add-to-image
[ -d iso ] || mkdir iso

# Create a FAT type image - a floppy disk in this case
# /dev/zero is a stream of endless nulls, output file is fat.img, blocksize is 1kb, and were having 14400 block - approx. 14.3 mb
dd if=/dev/zero of=fat.img bs=1k count=14400
# format the image as a floppy disk (doesnt really matter if its floppy/HDD/SSD, just for emu purposes) - with a size of 1.44MB
mformat -i fat.img -s 64 -t 225 -h 16 ::

# Create dirs on image where booting files will be stored
mmd -i fat.img ::/EFI
mmd -i fat.img ::/EFI/BOOT
mmd -i fat.img ::/EFI/thatloader

# Copy the files
cp thatloader_x64.efi bootx64.efi # This is done in order to start the boot manager automatically
mcopy -i fat.img bootx64.efi ::/EFI/BOOT
mcopy -i fat.img add-to-image/* ::/EFI/thatloader

# Create an ISO image with our EFI file and start uefi qemu with fat.img as the booting device
cp fat.img iso
xorriso -as mkisofs -R -f -e fat.img -no-emul-boot -o cdimage.iso iso
rm -rf iso
qemu-system-x86_64 -cpu qemu64 -bios ovmf/OVMF.fd -drive file=cdimage.iso,if=ide -net none -enable-kvm