include $(ROOTDIR)/rules.mk

PKG_NAME:=libevent
PKG_VERSION:=2.1.12

PKG_SOURCE:=libevent-$(PKG_VERSION)-stable.tar.gz
PKG_SOURCE_URL:=https://github.com/libevent/libevent/releases/download/release-$(PKG_VERSION)-stable
PKG_HASH:=92e6de1be9ec176428fd2367677e61ceffc2ee1cb119035037a27d346b0403bb
PKG_BUILD_DIR:=$(BUILD_DIR)/libevent-$(PKG_VERSION)-stable

PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk

CONFIGURE_ARGS += \
	--disable-static \
	--enable-shared \
	--disable-debug-mode \
	--disable-samples

$(eval $(call BuildPackage,libevent))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/lib
	$(CP) $(PKG_INSTALL_DIR)/lib/libevent-2.1.so* $(ROMFSDIR)/lib/
