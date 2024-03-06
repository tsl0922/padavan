include $(ROOTDIR)/rules.mk

PKG_NAME:=libuv
PKG_VERSION:=1.45.0

PKG_SOURCE:=$(PKG_NAME)-v$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://dist.libuv.org/dist/v$(PKG_VERSION)/
PKG_HASH:=f5b07f65a1e8166e47983a7ed1f42fae0bee08f7458142170c37332fc676a748
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-v$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

CMAKE_OPTIONS += -DBUILD_TESTING=OFF

$(eval $(call BuildPackage,libuv))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/lib
	$(CP) $(PKG_INSTALL_DIR)/lib/libuv.so* $(ROMFSDIR)/lib/
