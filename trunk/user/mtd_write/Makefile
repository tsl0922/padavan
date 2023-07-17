include $(ROOTDIR)/rules.mk

PKG_NAME:=mtd_write

include $(INCLUDE_DIR)/package.mk

$(eval $(call BuildPackage,mtd_write))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mtd_write $(ROMFSDIR)/bin/mtd_write
