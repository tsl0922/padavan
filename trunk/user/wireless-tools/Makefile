include $(ROOTDIR)/rules.mk

PKG_NAME:=wireless-tools

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

MAKE_INSTALL_FLAGS += PREFIX=$(STAGING_DIR)

$(eval $(call BuildPackage,wireless-tools))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/iwpriv $(ROMFSDIR)/bin/iwpriv
ifndef BUILD_STATIC
	$(INSTALL_DIR) $(ROMFSDIR)/lib
	$(CP) $(PKG_BUILD_DIR)/libiw.so* $(ROMFSDIR)/lib/
endif