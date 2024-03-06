include $(ROOTDIR)/rules.mk

PKG_NAME:=libogg
PKG_VERSION:=1.3.3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=http://downloads.xiph.org/releases/ogg/
PKG_HASH:=4f3fc6178a533d392064f14776b23c397ed4b9f48f5de297aba73b643f955c08

include $(INCLUDE_DIR)/package.mk

CONFIGURE_ARGS += \
	--enable-static \
	--disable-shared

$(eval $(call BuildPackage,libogg))

romfs:
