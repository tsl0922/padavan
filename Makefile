TOPDIR       = ${CURDIR}
SOURCE_DIR   = $(TOPDIR)/trunk
TEMPLATE_DIR = $(SOURCE_DIR)/configs/templates
PRODUCTS     = $(shell ls $(TEMPLATE_DIR) | sed 's/.config//g')
CONFIG       = $(SOURCE_DIR)/.config
TOOLCHAIN    := mipsel-linux-musl

all: build

build:
	$(MAKE) -C $(TOPDIR)/toolchain download CT_TARGET=$(TOOLCHAIN)
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
	@echo "CONFIG_CROSS_COMPILER_ROOT=$(TOPDIR)/toolchain/toolchain-mipsel" >> $(CONFIG)
	@echo "CONFIG_TOOLCHAIN=$(TOOLCHAIN)" >> $(CONFIG)
	@echo "CONFIG_CCACHE=y" >> $(CONFIG)
	@make build
