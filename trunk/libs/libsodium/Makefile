include $(ROOTDIR)/rules.mk

PKG_NAME:=libsodium
PKG_VERSION:=1.0.18

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://download.libsodium.org/libsodium/releases \
		https://github.com/jedisct1/libsodium/releases/download/$(PKG_VERSION)
PKG_HASH:=6f504490b342a4f8a4c4a02fc9b866cbef8622d5df4e5452b46be121e46636c1

include $(INCLUDE_DIR)/package.mk

CONFIGURE_ARGS += --disable-ssp

$(eval $(call BuildPackage,libsodium))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/lib
	$(CP) $(PKG_INSTALL_DIR)/lib/libsodium.so* $(ROMFSDIR)/lib/
