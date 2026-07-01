include $(ROOTDIR)/rules.mk

PKG_NAME:=libsodium
PKG_VERSION:=1.0.20

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://download.libsodium.org/libsodium/releases \
		https://github.com/jedisct1/libsodium/releases/download/$(PKG_VERSION)-RELEASE/
PKG_HASH:=ebb65ef6ca439333c2bb41a0c1990587288da07f6c7fd07cb3a18cc18d30ce19

include $(INCLUDE_DIR)/package.mk

CONFIGURE_ARGS += --disable-ssp

$(eval $(call BuildPackage,libsodium))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/lib
	$(CP) $(PKG_INSTALL_DIR)/lib/libsodium.so* $(ROMFSDIR)/lib/
