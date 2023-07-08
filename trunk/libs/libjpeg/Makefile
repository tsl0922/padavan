include $(ROOTDIR)/rules.mk

PKG_NAME:=jpeg
PKG_VERSION:=9c

PKG_SOURCE:=jpegsrc.$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.ijg.org/files/
PKG_HASH:=682aee469c3ca857c4c38c37a6edadbfca4b04d42e56613b11590ec6aa4a278d

include $(INCLUDE_DIR)/package.mk

CONFIGURE_ARGS += \
	--enable-static \
	--disable-shared

$(eval $(call BuildPackage,jpeg))

romfs:
