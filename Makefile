TOPDIR:=${CURDIR}
SOURCE_DIR:=$(TOPDIR)/trunk
TOOLCHAIN_DIR:=$(TOPDIR)/toolchain-mipsel
TOOLCHAIN_ROOT:=$(TOOLCHAIN_DIR)/toolchain-4.4.x
TOOLCHAIN_URL:=https://github.com/tsl0922/padavan/releases/download/toolchain/mipsel-linux-uclibc-gcc10.tar.xz
TEMPLATE_DIR:=$(SOURCE_DIR)/configs/templates
PRODUCTS:=$(shell ls $(TEMPLATE_DIR) | sed 's/.config//g')
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
		if [ -d $(TOOLCHAIN_ROOT) ]; then rm -rf $(TOOLCHAIN_ROOT); fi \
	)

toolchain/download:
	@if [ ! -d $(TOOLCHAIN_ROOT) ]; then \
		echo "Downloading toolchain..."; \
		mkdir -p $(TOOLCHAIN_ROOT); \
		curl -fSsLo- $(TOOLCHAIN_URL) | tar Jx -C $(TOOLCHAIN_ROOT); \
	fi

build: toolchain/download
	@if [ ! -f $(CONFIG) ]; then \
		echo "Please run 'make PRODUCT_NAME' to start build!"; \
		echo "Supported products: $(PRODUCTS)"; \
		exit 1; \
	fi
	$(MAKE) -C $(SOURCE_DIR)

clean:
	@if [ ! -f $(CONFIG) ]; then \
		echo "Project config file .config not found! Terminate."; \
		exit 1; \
	fi
	$(MAKE) -C $(SOURCE_DIR) clean
	@rm -f $(CONFIG)

.PHONY: $(PRODUCTS)
$(PRODUCTS):
	cp -f $(TEMPLATE_DIR)/$(@).config $(CONFIG)
	@echo "CONFIG_CROSS_COMPILER_ROOT=$(TOOLCHAIN_ROOT)" >> $(CONFIG)
	@echo "CONFIG_CCACHE=y" >> $(CONFIG)
	@make build
