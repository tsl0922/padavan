include $(ROOTDIR)/rules.mk

PKG_NAME:=udpxy

include $(INCLUDE_DIR)/package.mk

$(eval $(call BuildPackage,udpxy))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/udpxy $(ROMFSDIR)/sbin/udpxy