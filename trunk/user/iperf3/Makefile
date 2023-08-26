include $(ROOTDIR)/rules.mk

PKG_NAME:=iperf
PKG_VERSION:=3.11

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://downloads.es.net/pub/iperf
PKG_HASH:=de8cb409fad61a0574f4cb07eb19ce1159707403ac2dc01b5d175e91240b7e5f

PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk

TARGET_CFLAGS += -D_GNU_SOURCE
CONFIGURE_ARGS += \
	--disable-shared \
	--enable-static \
	--without-openssl

$(eval $(call BuildPackage,iperf))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/iperf3 $(ROMFSDIR)/bin/iperf3
