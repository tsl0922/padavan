include $(ROOTDIR)/rules.mk

PKG_NAME:=dns2tcp
PKG_VERSION:=1.1.0

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://codeload.github.com/zfl9/dns2tcp/tar.gz/v$(PKG_VERSION)?
PKG_HASH:=31eb878583e6b10bd11300be4307a20e4f611709f0ee5b220bd198b203a62840

include $(INCLUDE_DIR)/package.mk

$(eval $(call BuildPackage,dns2tcp))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/dns2tcp $(ROMFSDIR)/usr/bin/dns2tcp
