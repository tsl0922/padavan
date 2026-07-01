include $(ROOTDIR)/rules.mk

PKG_NAME:=dns2tcp
PKG_VERSION:=1.1.2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://codeload.github.com/zfl9/dns2tcp/tar.gz/v$(PKG_VERSION)?
PKG_HASH:=5e8c6302a1d32c16ae7d4b8e39cd9aad1f2d7e68fe18813e76cb1e48ec5940d2

include $(INCLUDE_DIR)/package.mk

$(eval $(call BuildPackage,dns2tcp))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/dns2tcp $(ROMFSDIR)/usr/bin/dns2tcp
