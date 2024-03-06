include $(ROOTDIR)/rules.mk

PKG_NAME:=mtr
PKG_VERSION:=0.95

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://codeload.github.com/traviscross/mtr/tar.gz/v$(PKG_VERSION)?
PKG_HASH:=12490fb660ba5fb34df8c06a0f62b4f9cbd11a584fc3f6eceda0a99124e8596f

PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk

CONFIGURE_ARGS += --without-gtk

$(eval $(call BuildPackage,mtr))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mtr $(ROMFSDIR)/usr/bin/mtr
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mtr-packet $(ROMFSDIR)/usr/bin/mtr-packet
