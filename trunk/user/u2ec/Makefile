include $(ROOTDIR)/rules.mk

PKG_NAME:=u2ec

include $(INCLUDE_DIR)/package.mk

TARGET_CFLAGS += -I$(STAGING_DIR)/include/shared
TARGET_LDFLAGS += -L$(STAGING_DIR)/lib

$(eval $(call BuildPackage,u2ec))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/u2ec $(ROMFSDIR)/usr/sbin/u2ec
