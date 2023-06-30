TOPDIR:=${CURDIR}
CT_DIR:=$(TOPDIR)/crosstool-ng
CT_PREFIX:=$(TOPDIR)/toolchain-mipsel
TOOLCHAIN_URL:=https://github.com/tsl0922/padavan/releases/download/toolchain/mipsel-linux-uclibc-gcc12.tar.xz

build:
	@echo "Building toolchain..."
	@(cd $(CT_DIR); \
		./bootstrap && \
		./configure --enable-local && \
		make && \
		./ct-ng mipsel-linux-uclibc && \
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