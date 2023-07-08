include $(ROOTDIR)/rules.mk

PKG_NAME:=libiconv
PKG_VERSION:=1.17

PKG_SOURCE:=libiconv-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@GNU/libiconv
PKG_HASH:=8f74213b56238c85a50a5329f77e06198771e70dd9a739779f4c02f65d971313
PKG_BUILD_DIR:=$(BUILD_DIR)/libiconv-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

CONFIGURE_ARGS += \
	--enable-shared \
	--disable-static \
	--disable-debug-mode \
	--enable-relocatable \
	--disable-nls

$(eval $(call BuildPackage,libiconv))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/lib
	$(CP) $(PKG_INSTALL_DIR)/lib/libiconv.so* $(ROMFSDIR)/lib/
