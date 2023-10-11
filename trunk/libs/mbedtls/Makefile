include $(ROOTDIR)/rules.mk

PKG_NAME:=mbedtls
PKG_VERSION:=2.28.4

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://codeload.github.com/ARMmbed/mbedtls/tar.gz/v$(PKG_VERSION)?
PKG_HASH:=578c4dcd15bbff3f5cd56aa07cd4f850fc733634e3d5947be4f7157d5bfd81ac

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

CMAKE_OPTIONS += \
	-DUSE_SHARED_MBEDTLS_LIBRARY=ON \
	-DUSE_STATIC_MBEDTLS_LIBRARY=OFF \
	-DENABLE_TESTING=OFF \
	-DENABLE_PROGRAMS=OFF \
	-DLINK_WITH_PTHREAD=ON \
	-DENABLE_ZLIB_SUPPORT=ON

$(eval $(call BuildPackage,mbedtls))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/lib
	$(CP) $(PKG_INSTALL_DIR)/lib/libmbedcrypto.so* $(ROMFSDIR)/lib/
	$(CP) $(PKG_INSTALL_DIR)/lib/libmbedtls.so* $(ROMFSDIR)/lib/
	$(CP) $(PKG_INSTALL_DIR)/lib/libmbedx509.so* $(ROMFSDIR)/lib/
