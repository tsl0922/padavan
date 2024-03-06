include $(ROOTDIR)/rules.mk

PKG_NAME:=infosvr

include $(INCLUDE_DIR)/package.mk

TARGET_CFLAGS += -I$(STAGING_DIR)/include/shared
TARGET_LDFLAGS += -L$(STAGING_DIR)/lib

$(eval $(call BuildPackage,infosvr))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/infosvr $(ROMFSDIR)/usr/sbin/infosvr
