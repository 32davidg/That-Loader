ARCH = x86_64
TARGET = thatloader_x64.efi
SRCS = $(wildcard src/*.c) 
CFLAGS = -Iinclude -Wall -Wextra -pedantic -Wno-unused-parameter -O2

include uefi/Makefile
