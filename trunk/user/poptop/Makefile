include $(ROOTDIR)/rules.mk

PKG_NAME:=pptpd
PKG_VERSION:=1.4.0

PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk

SRC_DIR=./pptpd-1.4.0

CONFIGURE_ARGS += --enable-bcrelay
CONFIGURE_VARS += ac_cv_header_libintl_h=no
MAKE_FLAGS += \
	COPTS="$(TARGET_CFLAGS)" \
	INSTALL="install"

$(eval $(call BuildPackage,pptpd))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/pptpd $(ROMFSDIR)/usr/sbin/pptpd
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/pptpctrl $(ROMFSDIR)/usr/sbin/pptpctrl
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/bcrelay $(ROMFSDIR)/usr/sbin/bcrelay
