include $(ROOTDIR)/rules.mk

PKG_NAME:=pppoe-relay

include $(INCLUDE_DIR)/package.mk

$(eval $(call BuildPackage,pppoe-relay))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/pppoe-relay $(ROMFSDIR)/usr/sbin/pppoe-relay
