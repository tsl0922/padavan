include $(ROOTDIR)/rules.mk

PKG_NAME:=libmnl
PKG_VERSION:=1.0.5

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:= \
	http://www.netfilter.org/projects/libmnl/files \
	ftp://ftp.netfilter.org/pub/libmnl
PKG_HASH:=274b9b919ef3152bfb3da3a13c950dd60d6e2bcd54230ffeca298d03b40d0525

include $(INCLUDE_DIR)/package.mk

CONFIGURE_ARGS += \
	--enable-static \
	--disable-shared

$(eval $(call BuildPackage,libmnl))

romfs:
