include $(ROOTDIR)/rules.mk

PKG_NAME:=libxml2
PKG_VERSION:=2.9.9

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=@GNOME/libxml2/$(basename $(PKG_VERSION))
PKG_HASH:=58a5c05a2951f8b47656b676ce1017921a29f6b1419c45e3baed0d6435ba03f5

include $(INCLUDE_DIR)/package.mk

CONFIGURE_ARGS += \
	--enable-static \
	--disable-shared \
	$(if $(CONFIG_IPV6),--enable-ipv6,--disable-ipv6) \
	--without-catalog \
	--without-debug \
	--without-docbook \
	--without-ftp \
	--without-http \
	--without-legacy \
	--without-lzma \
	--without-pattern \
	--without-push \
	--without-python \
	--without-readline \
	--without-regexps \
	--without-iconv \
	--without-iso8859x \
	--with-c14n \
	--with-html \
	--with-output \
	--with-reader \
	--with-sax1 \
	--with-schemas \
	--with-threads \
	--with-tree \
	--with-valid \
	--with-writer \
	--with-xinclude \
	--with-xpath \
	--with-xptr \
	--with-zlib=$(STAGING_DIR)

$(eval $(call BuildPackage,libxml2))

romfs:
