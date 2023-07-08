include $(ROOTDIR)/rules.mk

PKG_NAME:=libusb
PKG_VERSION:=0.1.12

include $(INCLUDE_DIR)/package.mk

SRC_DIR=./libusb-0.1.12

CONFIGURE_ARGS += \
	--enable-static \
	--disable-shared \
	--disable-build-docs \
	--enable-debug=no
TARGET_CFLAGS += -Wno-format-truncation

$(eval $(call BuildPackage,libusb))

romfs:
