include $(ROOTDIR)/rules.mk

PKG_NAME:=hdparm

include $(INCLUDE_DIR)/package.mk

$(eval $(call BuildPackage,hdparm))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/hdparm $(ROMFSDIR)/sbin/hdparm
