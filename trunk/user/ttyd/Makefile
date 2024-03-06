include $(ROOTDIR)/rules.mk

PKG_NAME:=ttyd
PKG_VERSION:=1.7.4

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://codeload.github.com/tsl0922/ttyd/tar.gz/$(PKG_VERSION)?
PKG_HASH:=300d8cef4b0b32b0ec30d7bf4d3721a5d180e22607f9467a95ab7b6d9652ca9b

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

$(eval $(call BuildPackage,ttyd))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ttyd $(ROMFSDIR)/usr/bin/ttyd
	$(INSTALL_BIN) ./ttyd.sh $(ROMFSDIR)/usr/bin/ttyd.sh
