include $(ROOTDIR)/rules.mk

PKG_NAME:=pppoe

include $(INCLUDE_DIR)/package.mk

TARGET_CFLAGS += $(if $(CONFIG_RAETH_GMAC2),,-DUSE_SINGLE_MAC)

$(eval $(call BuildPackage,pppoe))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/pppoe-discovery $(ROMFSDIR)/usr/bin/pppoe-discovery
	$(INSTALL_BIN) ./detect.sh $(ROMFSDIR)/usr/bin/detect.sh
	$(INSTALL_BIN) ./mwan.sh $(ROMFSDIR)/usr/bin/mwan.sh