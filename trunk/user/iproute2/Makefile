include $(ROOTDIR)/rules.mk

PKG_NAME:=iproute2
PKG_VERSION:=5.12.0

include $(INCLUDE_DIR)/package.mk

SRC_DIR=./iproute2-5.12.0

$(eval $(call BuildPackage,iproute2))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ip/ip $(ROMFSDIR)/bin/ip
ifeq ($(CONFIG_FIRMWARE_INCLUDE_SQM),y)
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/tc/tc $(ROMFSDIR)/bin/tc
endif
