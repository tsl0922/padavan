include $(ROOTDIR)/rules.mk

PKG_NAME:=igmpproxy
PKG_VERSION:=0.4

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://github.com/pali/igmpproxy/releases/download/${PKG_VERSION}/
PKG_HASH:=afa4b75a823b82f71ce99f33eae4e8136b906ae8a5ede5caaad93bac38cdae24

PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk

$(eval $(call BuildPackage,igmpproxy))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/igmpproxy $(ROMFSDIR)/bin/igmpproxy
