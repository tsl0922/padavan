include $(ROOTDIR)/rules.mk

PKG_NAME:=c-ares
PKG_VERSION:=1.19.1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://c-ares.org/download
PKG_HASH:=321700399b72ed0e037d0074c629e7741f6b2ec2dda92956abe3e9671d3e268e

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

$(eval $(call BuildPackage,c-ares))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/lib
	$(CP) $(PKG_INSTALL_DIR)/lib/libcares.so* $(ROMFSDIR)/lib/
