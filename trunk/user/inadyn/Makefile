include $(ROOTDIR)/rules.mk

PKG_NAME:=inadyn
PKG_VERSION:=2.12.0

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=https://github.com/troglobit/inadyn/releases/download/v$(PKG_VERSION)
PKG_HASH:=e64c4386b6f42bbace589b847f22823a755b59f14a277a5f556fef0d99069245

PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk

CONFIGURE_ARGS += --enable-openssl

$(eval $(call BuildPackage,inadyn))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/inadyn $(ROMFSDIR)/bin/inadyn
