include $(ROOTDIR)/rules.mk

PKG_NAME:=libexif
PKG_VERSION:=0.6.25

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=https://github.com/libexif/libexif/releases/download/v$(PKG_VERSION)
PKG_HASH:=7c9eba99aed3e6594d8c3e85861f1c6aaf450c218621528bc989d3b3e7a26307

include $(INCLUDE_DIR)/package.mk

CONFIGURE_ARGS += \
	--enable-static \
	--disable-shared \
	--disable-nls \
	--disable-docs

$(eval $(call BuildPackage,libexif))

romfs:
