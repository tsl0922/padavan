include $(ROOTDIR)/rules.mk

PKG_NAME:=libdisk
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

TARGET_CFLAGS += -I$(STAGING_DIR)/include/shared
TARGET_LDFLAGS += -L$(STAGING_DIR)/lib

$(eval $(call BuildPackage,libdisk))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/lib
	$(CP) $(PKG_BUILD_DIR)/libdisk.so $(ROMFSDIR)/lib/
ifneq ($(filter y,$(CONFIG_FIRMWARE_INCLUDE_SMBD) $(CONFIG_FIRMWARE_INCLUDE_FTPD)),)
	$(INSTALL_DIR) $(ROMFSDIR)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/test_share $(ROMFSDIR)/sbin/test_share
endif
