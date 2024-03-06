include $(ROOTDIR)/rules.mk

PKG_NAME:=libwebsockets
PKG_VERSION:=4.3.2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_URL:=https://codeload.github.com/warmcat/libwebsockets/tar.gz/v$(PKG_VERSION)?
PKG_HASH:=6a85a1bccf25acc7e8e5383e4934c9b32a102880d1e4c37c70b27ae2a42406e1

PKG_SOURCE_VERSION:=v$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

CMAKE_OPTIONS += \
	-DLWS_WITH_LIBUV=ON \
	-DLWS_WITHOUT_BUILTIN_SHA1=ON \
	-DLWS_WITHOUT_TESTAPPS=ON \
	-DLWS_WITH_CUSTOM_HEADERS=ON \
	-DLWS_IPV6=ON \
	-DLWS_ROLE_RAW_FILE=OFF \
	-DLWS_WITH_HTTP2=OFF \
	-DLWS_WITH_HTTP_BASIC_AUTH=OFF \
	-DLWS_WITH_UDP=OFF \
	-DLWS_WITHOUT_CLIENT=ON

TARGET_CFLAGS += -Wno-error=enum-int-mismatch

$(eval $(call BuildPackage,libwebsockets))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/lib
	$(CP) $(PKG_INSTALL_DIR)/lib/libwebsockets.so* $(ROMFSDIR)/lib/
	$(CP) $(PKG_INSTALL_DIR)/lib/libwebsockets-evlib_uv.so* $(ROMFSDIR)/lib/
