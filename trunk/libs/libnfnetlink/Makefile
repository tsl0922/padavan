include $(ROOTDIR)/rules.mk

PKG_NAME:=libnfnetlink
PKG_VERSION:=1.0.2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:= \
	http://www.netfilter.org/projects/libnfnetlink/files/ \
	ftp://ftp.netfilter.org/pub/libnfnetlink/
PKG_HASH:=b064c7c3d426efb4786e60a8e6859b82ee2f2c5e49ffeea640cfe4fe33cbc376

include $(INCLUDE_DIR)/package.mk

CONFIGURE_ARGS += \
	--enable-static \
	--disable-shared

$(eval $(call BuildPackage,libmnl))

romfs:
