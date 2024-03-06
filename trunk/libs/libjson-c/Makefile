include $(ROOTDIR)/rules.mk

PKG_NAME:=json-c
PKG_VERSION:=0.16

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-nodoc.tar.gz
PKG_SOURCE_URL:=https://s3.amazonaws.com/json-c_releases/releases/
PKG_HASH:=ac8a3dd6820daaca579b23fbc74664310fbc3d67f52f6707cda67d21dde5570f

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

CMAKE_OPTIONS += -DDISABLE_EXTRA_LIBS=TRUE

$(eval $(call BuildPackage,json-c))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/lib
	$(CP) $(PKG_INSTALL_DIR)/lib/libjson-c.so* $(ROMFSDIR)/lib/
