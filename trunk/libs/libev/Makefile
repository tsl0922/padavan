include $(ROOTDIR)/rules.mk

PKG_NAME:=libev
PKG_VERSION:=4.33

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://dist.schmorp.de/libev/Attic/
PKG_HASH:=507eb7b8d1015fbec5b935f34ebed15bf346bed04a11ab82b8eee848c4205aea

include $(INCLUDE_DIR)/package.mk

$(eval $(call BuildPackage,libev))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/lib
	$(CP) $(PKG_INSTALL_DIR)/lib/libev.so* $(ROMFSDIR)/lib/
