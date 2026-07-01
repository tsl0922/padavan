include $(ROOTDIR)/rules.mk

PKG_NAME:=libusb
PKG_VERSION:=1.0.28

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=\
  https://github.com/libusb/libusb/releases/download/v$(PKG_VERSION) \
  @SF/$(PKG_NAME)
PKG_HASH:=966bb0d231f94a474eaae2e67da5ec844d3527a1f386456394ff432580634b29

PKG_FIXUP:=autoreconf
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
