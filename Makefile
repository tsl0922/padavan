TOPDIR:=${CURDIR}
SOURCE_DIR:=$(TOPDIR)/trunk
TOOLCHAIN_DIR:=$(TOPDIR)/toolchain-mipsel
TEMPLATE_DIR:=$(SOURCE_DIR)/configs/templates
CONFIG:=$(SOURCE_DIR)/.config

.PHONY: all toolchain build clean

all: build

toolchain:
	@(cd $(TOOLCHAIN_DIR); ./dl_toolchain.sh)

toolchain/build:
	@(cd $(TOOLCHAIN_DIR); ./build_toolchain)

toolchain/clean:
	@(cd $(TOOLCHAIN_DIR); ./clean_toolchain)

build: toolchain
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