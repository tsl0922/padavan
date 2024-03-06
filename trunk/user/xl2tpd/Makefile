include $(ROOTDIR)/rules.mk

PKG_NAME:=xl2tpd

include $(INCLUDE_DIR)/package.mk

$(eval $(call BuildPackage,xl2tpd))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/xl2tpd $(ROMFSDIR)/usr/sbin/xl2tpd
