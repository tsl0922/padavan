include $(ROOTDIR)/rules.mk

PKG_NAME:=mbedtls
PKG_VERSION:=2.28.8

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://codeload.github.com/ARMmbed/mbedtls/tar.gz/v$(PKG_VERSION)?
PKG_HASH:=4fef7de0d8d542510d726d643350acb3cdb9dc76ad45611b59c9aa08372b4213

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
