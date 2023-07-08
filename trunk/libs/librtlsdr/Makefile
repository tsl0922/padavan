include $(ROOTDIR)/rules.mk

PKG_NAME:=librtlsdr
PKG_VERSION:=0.6.0

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://github.com/steve-m/librtlsdr/archive/refs/tags/
PKG_SOURCE_URL_FILE:=$(PKG_VERSION).tar.gz
PKG_HASH:=80a5155f3505bca8f1b808f8414d7dcd7c459b662a1cde84d3a2629a6e72ae55

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

$(eval $(call BuildPackage,librtlsdr))

romfs:
ifeq ($(CONFIG_FIRMWARE_INCLUDE_RTL_SDR),y)
	$(INSTALL_DIR) $(ROMFSDIR)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/bin/rtl_adsb $(ROMFSDIR)/usr/bin/rtl_adsb
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/bin/rtl_eeprom $(ROMFSDIR)/usr/bin/rtl_eeprom
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/bin/rtl_fm $(ROMFSDIR)/usr/bin/rtl_fm
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/bin/rtl_power $(ROMFSDIR)/usr/bin/rtl_power
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/bin/rtl_sdr $(ROMFSDIR)/usr/bin/rtl_sdr
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/bin/rtl_tcp $(ROMFSDIR)/usr/bin/rtl_tcp
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/bin/rtl_test $(ROMFSDIR)/usr/bin/rtl_test
endif
	$(INSTALL_DIR) $(ROMFSDIR)/lib
	$(CP) $(PKG_INSTALL_DIR)/lib/librtlsdr.so* $(ROMFSDIR)/lib/
