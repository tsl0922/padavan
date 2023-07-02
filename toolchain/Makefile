TOPDIR        = ${CURDIR}
CT_DIR        = $(TOPDIR)/crosstool-ng
CT_PREFIX     = $(TOPDIR)/toolchain-mipsel
CT_TARGET     := mipsel-linux-uclibc
TOOLCHAIN_URL := https://github.com/tsl0922/padavan/releases/download/toolchain/$(CT_TARGET).tar.xz

all: build

build:
	@echo "Building toolchain..."
	@(cd $(CT_DIR); \
		./bootstrap && \
		./configure --enable-local && \
		make && \
		./ct-ng $(CT_TARGET) && \
		./ct-ng build \
	)

clean:
	@(cd $(CT_DIR); \
		if [ -f ct-ng ]; then ./ct-ng distclean; fi; \
		if [ -f Makefile ]; then make distclean; fi; \
		if [ -d $(CT_PREFIX) ]; then rm -rf $(CT_PREFIX); fi \
	)

download:
	@if [ ! -d $(CT_PREFIX) ]; then \
		echo "Downloading toolchain..."; \
		mkdir -p $(CT_PREFIX); \
		curl -fSsLo- $(TOOLCHAIN_URL) | tar Jx -C $(CT_PREFIX); \
	fi
ifeq ($(CT_TARGET),mipsel-linux-musl)
	@if [ ! -f $(CT_PREFIX)/$(CT_TARGET)/sysroot/usr/include/sys/queue.h ]; then \
		echo "Install sys/queue.h..."; \
		curl -fSsL -o $(CT_PREFIX)/$(CT_TARGET)/sysroot/usr/include/sys/queue.h \
			"https://sourceware.org/git/?p=glibc.git;a=blob_plain;f=misc/sys/queue.h;hb=HEAD" ; \
	fi
endif