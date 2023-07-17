include $(ROOTDIR)/rules.mk

PKG_NAME:=802.1x

include $(INCLUDE_DIR)/package.mk

$(eval $(call BuildPackage,802.1x))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/rt2860apd $(ROMFSDIR)/bin/rt2860apd
	$(LN) rt2860apd $(ROMFSDIR)/bin/rtinicapd