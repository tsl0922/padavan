include $(ROOTDIR)/rules.mk

PKG_NAME:=miniupnpd
PKG_VERSION:=2.0

include $(INCLUDE_DIR)/package.mk

SRC_DIR=./miniupnpd-2.x

# disable support IPv6 yet, because no profit w/o IGDv2
# e.g. Windows not supported IGDv2 and UPnP not worked
ENABLE_IPV6:=0

MAKE_FLAGS += \
	-f Makefile.linux \
	ENABLE_IPV6=$(ENABLE_IPV6)

$(eval $(call BuildPackage,miniupnpd))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/miniupnpd $(ROMFSDIR)/usr/bin/miniupnpd
