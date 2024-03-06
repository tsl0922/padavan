include $(ROOTDIR)/rules.mk

PKG_NAME:=wireless-tools
PKG_VERSION:=30

include $(INCLUDE_DIR)/package.mk

MAKE_INSTALL_FLAGS += PREFIX=$(STAGING_DIR)

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(TARGET_CONFIGURE_OPTS) \
		CFLAGS="$(TARGET_CFLAGS) -I." \
		BUILD_WE_ESSENTIAL=y \
		LIBS="-lm -Wl,--gc-sections" \
		libiw.so.$(PKG_VERSION) iwmulticall
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(MAKE_INSTALL_FLAGS) \
		install-dynamic install-hdr
endef

$(eval $(call BuildPackage,wireless-tools))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/iwmulticall $(ROMFSDIR)/bin/iwconfig
	$(LN) iwconfig $(ROMFSDIR)/bin/iwlist
	$(LN) iwconfig $(ROMFSDIR)/bin/iwpriv
ifndef BUILD_STATIC
	$(INSTALL_DIR) $(ROMFSDIR)/lib
	$(CP) $(PKG_BUILD_DIR)/libiw.so* $(ROMFSDIR)/lib/
endif