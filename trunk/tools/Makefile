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
SSTRIP_NAME = ELFkickers-3.1
GO_VERSION = 1.18.6

CFLAGS    = -Wall -O2
LDFLAGS   =

THISDIR = $(shell pwd)
-include $(LINUX_CONFIG)

all: go_build
	make -C mkimage
	mkdir -p sstrip
	make -C $(SSTRIP_NAME)
	make -C lzma
	make -C mksquashfs_xz

go_build:
	mkdir -p $(THISDIR)/go
	( if [ ! -f $(THISDIR)/go/go$(GO_VERSION).linux-amd64.tar.gz ]; then \
	curl --create-dirs -L https://go.dev/dl/go$(GO_VERSION).linux-amd64.tar.gz -o $(THISDIR)/go/go$(GO_VERSION).linux-amd64.tar.gz ; \
	fi )
	( if [ ! -d $(THISDIR)/go/go ]; then \
	tar zxfv $(THISDIR)/go/go$(GO_VERSION).linux-amd64.tar.gz -C go ; \
	fi )
clean:
	if [ -f $(SSTRIP_NAME)/Makefile ] ; then \
		make -C $(SSTRIP_NAME) clean ; \
	fi ;
	rm -rf sstrip
	make -C mkimage clean
	make -C lzma clean
	make -C mksquashfs_xz clean
