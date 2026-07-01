include $(ROOTDIR)/rules.mk

PKG_NAME:=json-c
PKG_VERSION:=0.18

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://s3.amazonaws.com/json-c_releases/releases/
PKG_HASH:=876ab046479166b869afc6896d288183bbc0e5843f141200c677b3e8dfb11724

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

CMAKE_OPTIONS += -DDISABLE_EXTRA_LIBS=TRUE

$(eval $(call BuildPackage,json-c))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/lib
	$(CP) $(PKG_INSTALL_DIR)/lib/libjson-c.so* $(ROMFSDIR)/lib/
