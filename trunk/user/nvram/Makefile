include $(ROOTDIR)/rules.mk

PKG_NAME:=nvram

include $(INCLUDE_DIR)/package.mk

TARGET_CFLAGS += -I$(STAGING_DIR)/include/shared
TARGET_LDFLAGS += -L$(STAGING_DIR)/lib

$(eval $(call BuildPackage,nvram))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/nvram $(ROMFSDIR)/usr/sbin/nvram
