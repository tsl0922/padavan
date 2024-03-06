include $(ROOTDIR)/rules.mk

PKG_NAME:=v2ray
PKG_VERSION:=5.5.0

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://codeload.github.com/v2fly/v2ray-core/tar.gz/v$(PKG_VERSION)?
PKG_HASH:=b9110ff10d87b245efd42b5a93beb4a3e5138fcfe92d09cadfb6a72cc4e1c91c

PKG_BUILD_DIR:=$(BUILD_DIR)/v2ray-core-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Build/Configure
endef

define Build/Compile
	(cd $(PKG_BUILD_DIR); \
		GOOS=linux GOARCH=mipsle CGO_ENABLED=0 \
		go build -v -o v2ray \
		-trimpath -ldflags "-s -w -buildid=" \
		./main; \
	)
endef

$(eval $(call BuildPackage,v2ray))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/v2ray $(ROMFSDIR)/usr/bin/v2ray
