include $(ROOTDIR)/rules.mk

PKG_NAME:=htop
PKG_VERSION:=3.2.2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://codeload.github.com/htop-dev/htop/tar.gz/$(PKG_VERSION)?
PKG_HASH:=3829c742a835a0426db41bb039d1b976420c21ec65e93b35cd9bfd2d57f44ac8

PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk

CONFIGURE_ARGS += \
	--enable-unicode \
	HTOP_NCURSESW_CONFIG_SCRIPT="$(STAGEDIR)/bin/ncursesw6-config"

$(eval $(call BuildPackage,htop))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/htop $(ROMFSDIR)/usr/bin/htop
