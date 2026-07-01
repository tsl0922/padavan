include $(ROOTDIR)/rules.mk

PKG_NAME:=libtirpc
PKG_VERSION:=1.3.5

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=@SF/$(PKG_NAME)
PKG_HASH:=9b31370e5a38d3391bf37edfa22498e28fe2142467ae6be7a17c9068ec0bf12f

PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk

CONFIGURE_ARGS += \
	--enable-shared \
	--disable-static \
	--disable-gssapi

$(eval $(call BuildPackage,libtirpc))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/lib
	$(CP) $(PKG_INSTALL_DIR)/lib/libtirpc.so* $(ROMFSDIR)/lib/
	$(INSTALL_DIR) $(ROMFSDIR)/etc_ro
	$(INSTALL_DATA) $(PKG_INSTALL_DIR)/etc/netconfig $(ROMFSDIR)/etc_ro/netconfig
