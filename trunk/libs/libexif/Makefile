include $(ROOTDIR)/rules.mk

PKG_NAME:=libexif
PKG_VERSION:=0.6.24

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=https://github.com/libexif/libexif/releases/download/v$(PKG_VERSION)
PKG_HASH:=d47564c433b733d83b6704c70477e0a4067811d184ec565258ac563d8223f6ae

include $(INCLUDE_DIR)/package.mk

CONFIGURE_ARGS += \
	--enable-static \
	--disable-shared \
	--disable-nls \
	--disable-docs

$(eval $(call BuildPackage,libexif))

romfs:
