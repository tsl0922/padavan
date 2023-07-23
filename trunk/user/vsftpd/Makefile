include $(ROOTDIR)/rules.mk

PKG_NAME:=vsftpd

include $(INCLUDE_DIR)/package.mk

TARGET_CFLAGS += \
	-I$(STAGING_DIR)/include/shared \
	-I$(STAGING_DIR)/include/libdisk

TARGET_LDFLAGS += -L$(STAGING_DIR)/lib

$(eval $(call BuildPackage,vsftpd))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/usr/share/empty
	$(INSTALL_DIR) $(ROMFSDIR)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/vsftpd $(ROMFSDIR)/sbin/vsftpd
