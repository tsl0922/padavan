include $(ROOTDIR)/rules.mk

PKG_NAME:=ppp
PKG_VERSION:=2.4.7

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

SRC_DIR=./ppp-2.4.7

MAKE_FLAGS += \
	$(if $(CONFIG_IPV6),HAVE_INET6=y,) \
	COPTS="$(TARGET_CFLAGS)" \
	INSTALL="install"

$(eval $(call BuildPackage,ppp))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/pppd/pppd $(ROMFSDIR)/usr/sbin/pppd
	$(INSTALL_DIR) $(ROMFSDIR)/usr/lib/pppd
	$(CP) $(PKG_BUILD_DIR)/pppd/plugins/rp-pppoe/rp-pppoe.so $(ROMFSDIR)/usr/lib/pppd/
	$(CP) $(PKG_BUILD_DIR)/pppd/plugins/pppol2tp/pppol2tp.so $(ROMFSDIR)/usr/lib/pppd/
	$(CP) $(PKG_BUILD_DIR)/pppd/plugins/pptp/pptp.so $(ROMFSDIR)/usr/lib/pppd/
