include $(ROOTDIR)/rules.mk

PKG_NAME:=confuse
PKG_VERSION:=3.3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=https://github.com/martinh/libconfuse/releases/download/v$(PKG_VERSION)
PKG_HASH:=1dd50a0320e135a55025b23fcdbb3f0a81913b6d0b0a9df8cc2fdf3b3dc67010

include $(INCLUDE_DIR)/package.mk

CONFIGURE_ARGS += \
	--disable-examples \
	--enable-static \
	--disable-shared

$(eval $(call BuildPackage,confuse))

romfs:
