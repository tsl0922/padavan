include $(ROOTDIR)/rules.mk

PKG_NAME:=jpeg
PKG_VERSION:=9f

PKG_SOURCE:=jpegsrc.$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.ijg.org/files/
PKG_HASH:=04705c110cb2469caa79fb71fba3d7bf834914706e9641a4589485c1f832565b

include $(INCLUDE_DIR)/package.mk

CONFIGURE_ARGS += \
	--enable-static \
	--disable-shared

$(eval $(call BuildPackage,jpeg))

romfs:
