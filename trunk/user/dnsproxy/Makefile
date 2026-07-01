include $(ROOTDIR)/rules.mk

PKG_NAME:=dnsproxy
PKG_VERSION:=1.1.1

include $(INCLUDE_DIR)/package.mk

SRC_DIR=./dnsproxy-1.1.1

$(eval $(call BuildPackage,dnsproxy))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/dnsproxy $(ROMFSDIR)/usr/bin/dnsproxy
