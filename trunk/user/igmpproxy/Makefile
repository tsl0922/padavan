include $(ROOTDIR)/rules.mk

PKG_NAME:=igmpproxy
PKG_VERSION:=0.3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://github.com/pali/igmpproxy/releases/download/${PKG_VERSION}/
PKG_HASH:=d1fc244cb2fbbf99f720bda3e841fe59ece9b6919073790b4b892739b1b844eb

PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk

$(eval $(call BuildPackage,igmpproxy))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/igmpproxy $(ROMFSDIR)/bin/igmpproxy
