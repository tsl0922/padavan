include $(ROOTDIR)/rules.mk

PKG_NAME:=rp-l2tp
PKG_VERSION:=0.5-rtn

include $(INCLUDE_DIR)/package.mk

SRC_DIR=./rp-l2tp-0.5-rtn

CONFIGURE_ARGS += --sysconfdir=/etc
TARGET_CFLAGS += \
	-I$(KERNEL_HEADERS_PATH) \
	-DPPPOL2TP_V1

$(eval $(call BuildPackage,rp-l2tp))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/l2tpd $(ROMFSDIR)/usr/sbin/l2tpd
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/handlers/l2tp-control $(ROMFSDIR)/usr/sbin/l2tp-control
	$(INSTALL_DIR) $(ROMFSDIR)/usr/lib/l2tp
	$(CP) $(PKG_BUILD_DIR)/handlers/cmd.so $(ROMFSDIR)/usr/lib/l2tp/
	$(CP) $(PKG_BUILD_DIR)/handlers/sync-pppd.so $(ROMFSDIR)/usr/lib/l2tp/
