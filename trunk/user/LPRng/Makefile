include $(ROOTDIR)/rules.mk

PKG_NAME:=LPRng
PKG_JOBS:=-j1

include $(INCLUDE_DIR)/package.mk

TARGET_CFLAGS += -I$(STAGING_DIR)/include/shared
TARGET_LDFLAGS += -L$(STAGING_DIR)/lib -lshared

$(eval $(call BuildPackage,LPRng))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/lpd $(ROMFSDIR)/usr/sbin/lpd
