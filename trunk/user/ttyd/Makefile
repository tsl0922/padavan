include $(ROOTDIR)/rules.mk

PKG_NAME:=ttyd
PKG_VERSION:=1.7.7

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://codeload.github.com/tsl0922/ttyd/tar.gz/$(PKG_VERSION)?
PKG_HASH:=039dd995229377caee919898b7bd54484accec3bba49c118e2d5cd6ec51e3650

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

$(eval $(call BuildPackage,ttyd))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ttyd $(ROMFSDIR)/usr/bin/ttyd
	$(INSTALL_BIN) ./ttyd.sh $(ROMFSDIR)/usr/bin/ttyd.sh
