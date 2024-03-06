include $(ROOTDIR)/rules.mk

PKG_NAME:=libvorbis
PKG_VERSION:=1.3.6

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=http://downloads.xiph.org/releases/vorbis/
PKG_HASH:=af00bb5a784e7c9e69f56823de4637c350643deedaf333d0fa86ecdba6fcb415

include $(INCLUDE_DIR)/package.mk

CONFIGURE_ARGS += \
	--enable-static \
	--disable-shared \
	--with-ogg-libraries=$(STAGING_DIR)/lib \
	--disable-oggtest

$(eval $(call BuildPackage,libvorbis))

romfs:
