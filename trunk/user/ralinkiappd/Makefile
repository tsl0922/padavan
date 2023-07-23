include $(ROOTDIR)/rules.mk

PKG_NAME:=ralinkiappd

include $(INCLUDE_DIR)/package.mk

$(eval $(call BuildPackage,ralinkiappd))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ralinkiappd $(ROMFSDIR)/sbin/ralinkiappd
	$(INSTALL_DIR) $(ROMFSDIR)/usr/bin
	$(INSTALL_BIN) ./iappd.sh $(ROMFSDIR)/usr/bin/iappd.sh
