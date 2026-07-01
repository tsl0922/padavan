include $(ROOTDIR)/rules.mk

PKG_NAME:=zlib
PKG_VERSION:=1.3.1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=https://github.com/madler/zlib/releases/download/v$(PKG_VERSION)
PKG_HASH:=38ef96b8dfe510d42707d9c781877914792541133e1870841463bfa73f883e32

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

$(eval $(call BuildPackage,zlib))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/lib
	$(CP) $(PKG_INSTALL_DIR)/lib/libz.so* $(ROMFSDIR)/lib/
