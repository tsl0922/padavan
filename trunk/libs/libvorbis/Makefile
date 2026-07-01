include $(ROOTDIR)/rules.mk

PKG_NAME:=libvorbis
PKG_VERSION:=1.3.7

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=http://downloads.xiph.org/releases/vorbis/
PKG_HASH:=b33cc4934322bcbf6efcbacf49e3ca01aadbea4114ec9589d1b1e9d20f72954b

include $(INCLUDE_DIR)/package.mk

CONFIGURE_ARGS += \
	--enable-static \
	--disable-shared \
	--with-ogg-libraries=$(STAGING_DIR)/lib \
	--disable-oggtest

$(eval $(call BuildPackage,libvorbis))

romfs:
