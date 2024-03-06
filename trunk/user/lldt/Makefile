include $(ROOTDIR)/rules.mk

PKG_NAME:=lldt

include $(INCLUDE_DIR)/package.mk

TARGET_CFLAGS += \
	$(if $(CONFIG_IPV6),-DUSE_IPV6,) \
	-I$(STAGING_DIR)/include/shared
TARGET_LDFLAGS += -L$(STAGING_DIR)/lib

$(eval $(call BuildPackage,lldt))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/lld2d $(ROMFSDIR)/bin/lld2d
	$(INSTALL_DIR) $(ROMFSDIR)/etc_ro
	$(INSTALL_DATA) ./lld2d.conf $(ROMFSDIR)/etc_ro/lld2d.conf
	$(INSTALL_DATA) ./icon.ico $(ROMFSDIR)/etc_ro/icon.ico
	$(LN) icon.ico $(ROMFSDIR)/etc_ro/icon.large.ico