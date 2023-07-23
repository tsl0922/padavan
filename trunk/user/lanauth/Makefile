include $(ROOTDIR)/rules.mk

PKG_NAME:=lanauth

include $(INCLUDE_DIR)/package.mk

$(eval $(call BuildPackage,lanauth))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/lanauth $(ROMFSDIR)/usr/sbin/lanauth
