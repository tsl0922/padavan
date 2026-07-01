include $(ROOTDIR)/rules.mk

PKG_NAME:=libevent
PKG_VERSION:=2.2.1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-alpha-dev.tar.gz
PKG_SOURCE_URL:=https://github.com/libevent/libevent/releases/download/release-$(PKG_VERSION)-alpha
PKG_HASH:=36d0726e570fc2ee61a0a27cfb6bf2799e14a28d030a7473a7a2411f7533d359
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)-alpha-dev

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
