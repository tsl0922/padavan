include $(ROOTDIR)/rules.mk

PKG_NAME:=chinadns-ng
PKG_VERSION:=2023.10.28

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://codeload.github.com/zfl9/chinadns-ng/tar.gz/$(PKG_VERSION)?
PKG_HASH:=8dbce6ec767b6d132c5625e5533f96c42310f8b67ce4ca963ea34a6797ae99b4

include $(INCLUDE_DIR)/package.mk

$(eval $(call BuildPackage,chinadns-ng))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/chinadns-ng $(ROMFSDIR)/usr/bin/chinadns-ng
