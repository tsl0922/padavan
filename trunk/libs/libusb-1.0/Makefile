include $(ROOTDIR)/rules.mk

PKG_NAME:=libusb
PKG_VERSION:=1.0.23

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=\
  https://github.com/libusb/libusb/releases/download/v$(PKG_VERSION) \
  @SF/$(PKG_NAME)
PKG_HASH:=db11c06e958a82dac52cf3c65cb4dd2c3f339c8a988665110e0d24d19312ad8d

include $(INCLUDE_DIR)/package.mk

CONFIGURE_ARGS += \
	--disable-static \
	--disable-log \
	--disable-udev \
	--disable-examples-build \
	--disable-tests-build

$(eval $(call BuildPackage,libusb))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/lib
	$(CP) $(PKG_INSTALL_DIR)/lib/libusb-1.0.so* $(ROMFSDIR)/lib/
