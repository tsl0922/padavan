# use host target
CC        = gcc
GCC       = gcc
CXX       = g++
AS        = as
AR        = ar
LD        = ld
NM        = nm
OBJCOPY   = objcopy
RANLIB    = ranlib
STRIP     = strip
STRIPTOOL = $(STRIP)

CFLAGS    = -Wall -O2
LDFLAGS   =

THISDIR = $(shell pwd)
-include $(LINUX_CONFIG)

all:
	make -C mkimage
	make -C sstrip
	make -C lzma
	make -C mksquashfs_xz

clean:
	make -C sstrip clean
	make -C mkimage clean
	make -C lzma clean
	make -C mksquashfs_xz clean
