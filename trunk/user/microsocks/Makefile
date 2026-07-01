include $(ROOTDIR)/rules.mk

PKG_NAME:=microsocks
PKG_VERSION:=1.0.4

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://codeload.github.com/rofl0r/microsocks/tar.gz/refs/tags/v$(PKG_VERSION)?
PKG_HASH:=130127a87f55870f18fbe47a64d9b9533020e2900802d36a0f6fd2b074313deb

include $(INCLUDE_DIR)/package.mk

$(eval $(call BuildPackage,microsocks))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/microsocks $(ROMFSDIR)/usr/bin/microsocks
