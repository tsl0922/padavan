include $(ROOTDIR)/rules.mk

PKG_NAME:=naiveproxy
PKG_VERSION:=114.0.5735.91-3

include $(INCLUDE_DIR)/package.mk

STATIC_FILE:=$(PKG_NAME)-v$(PKG_VERSION)-openwrt-mipsel_24kc-static.tar.xz
STATIC_SUBDIR:=$(PKG_NAME)-v$(PKG_VERSION)-openwrt-mipsel_24kc-static
define Download/$(PKG_NAME)-static
	URL:=https://github.com/klzgrad/naiveproxy/releases/download/v$(PKG_VERSION)
	URL_FILE:=$(STATIC_FILE)
	FILE:=$(STATIC_FILE)
	HASH:=8737c4a796ff2ecee3afbb42c345d313baeb774742c440b0d97a1124896f45b3
endef

define Build/Prepare
	tar xf $(DL_DIR)/$(STATIC_FILE) -C $(PKG_BUILD_DIR)
endef

define Build/Configure
endef

define Build/Compile
	$(TARGET_CROSS)strip $(PKG_BUILD_DIR)/$(STATIC_SUBDIR)/naive
endef

$(eval $(call Download,$(PKG_NAME)-static))
$(eval $(call BuildPackage,naiveproxy))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(STATIC_SUBDIR)/naive $(ROMFSDIR)/usr/bin/naive