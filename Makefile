TOPDIR:=${CURDIR}
SOURCE_DIR:=$(TOPDIR)/trunk
TOOLCHAIN_DIR:=$(TOPDIR)/toolchain-mipsel
TOOLCHAIN_URL:=https://github.com/tsl0922/padavan/releases/download/toolchain/mipsel-linux-uclibc-gcc10.tar.xz
TEMPLATE_DIR:=$(SOURCE_DIR)/configs/templates
CONFIG:=$(SOURCE_DIR)/.config

all: build

toolchain/build:
	@echo "Building toolchain..."
	@(cd $(TOOLCHAIN_DIR); \
		./bootstrap && \
		./configure --enable-local && \
		make && \
		./ct-ng mipsel-linux-uclibc && \
		./ct-ng build \
	)

toolchain/clean:
	@(cd $(TOOLCHAIN_DIR); \
		if [ -f ct-ng ]; then ./ct-ng distclean; fi; \
		if [ -f Makefile ]; then make distclean; fi; \
		if [ -d toolchain-4.4.x ]; then rm -rf toolchain-4.4.x; fi \
	)

toolchain/download:
	@if [ ! -d $(TOOLCHAIN_DIR)/toolchain-4.4.x ]; then \
		echo "Downloading toolchain..."; \
		mkdir -p $(TOOLCHAIN_DIR)/toolchain-4.4.x; \
		curl -fSsLo- $(TOOLCHAIN_URL) | tar Jx -C $(TOOLCHAIN_DIR)/toolchain-4.4.x; \
	fi

build: toolchain/download
	@if [ ! -f $(CONFIG) ]; then \
		echo "Please run 'make PRODUCT_NAME' to start build!"; \
		echo "Supported products: $(shell ls $(TEMPLATE_DIR) | sed 's/.config//g')"; \
		exit 1; \
	fi
	@(cd $(SOURCE_DIR); fakeroot ./build_firmware)

clean:
	@(cd $(SOURCE_DIR); ./clear_tree; rm -f $(CONFIG))

%:
	@if [ ! -f "$(TEMPLATE_DIR)/$(@).config" ] ; then \
		echo "Invalid build target: $(@) "; \
		exit 1; \
	fi
	cp -f $(TEMPLATE_DIR)/$(@).config $(CONFIG)
	@echo "CONFIG_TOOLCHAIN_DIR=$(TOOLCHAIN_DIR)" >> $(CONFIG)
	@make build