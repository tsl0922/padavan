TOPDIR        = ${CURDIR}
CT_VERSION    := 1.26.0
CT_DIR        := $(TOPDIR)/crosstool-ng-$(CT_VERSION)
CT_PREFIX     := $(TOPDIR)/toolchain-mipsel
CT_TARGET     := mipsel-linux-musl
CT_URL        := https://github.com/crosstool-ng/crosstool-ng/releases/download/crosstool-ng-$(CT_VERSION)/crosstool-ng-$(CT_VERSION).tar.xz
TOOLCHAIN_URL := https://github.com/tsl0922/padavan/releases/download/toolchain/$(CT_TARGET).tar.xz

all: build

build:
	@if [ ! -d $(CT_DIR) ]; then \
		echo "Downloading crosstool-ng..."; \
		curl -fSsLo- $(CT_URL) | tar Jx; \
	fi

	@echo "Building toolchain..."
	cp -r $(TOPDIR)/configs/$(CT_TARGET) $(CT_DIR)/samples
	@(cd $(CT_DIR); \
		./bootstrap && \
		./configure --enable-local && \
		make && \
		./ct-ng $(CT_TARGET) && \
		./ct-ng build \
	)

clean:
	@if [ -d $(CT_DIR) ]; then \
		cd $(CT_DIR); \
		if [ -f ct-ng ]; then ./ct-ng distclean; fi; \
		if [ -f Makefile ]; then make distclean; fi; \
	fi
	@if [ -d $(CT_PREFIX) ]; then rm -rf $(CT_PREFIX); fi

download:
	@if [ ! -d $(CT_PREFIX) ]; then \
		echo "Downloading toolchain..."; \
		mkdir -p $(CT_PREFIX); \
		curl -fSsLo- $(TOOLCHAIN_URL) | tar Jx -C $(CT_PREFIX); \
	fi
ifeq ($(CT_TARGET),mipsel-linux-musl)
	@if [ ! -f $(CT_PREFIX)/$(CT_TARGET)/sysroot/usr/include/sys/queue.h ]; then \
		echo "Installing sys/queue.h..."; \
		curl -fSsL -o $(CT_PREFIX)/$(CT_TARGET)/sysroot/usr/include/sys/queue.h \
			"https://sourceware.org/git/?p=glibc.git;a=blob_plain;f=misc/sys/queue.h;hb=HEAD" ; \
	fi
endif