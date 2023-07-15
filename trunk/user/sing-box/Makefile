include $(ROOTDIR)/rules.mk

PKG_NAME:=sing-box
PKG_VERSION:=1.3.0

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://codeload.github.com/SagerNet/sing-box/tar.gz/v$(PKG_VERSION)?
PKG_HASH:=e89ffdadf11d4b34d12b2c0659efea0f0786b2396ea4638c61a06e7772783ca5

include $(INCLUDE_DIR)/package.mk

define Build/Configure
endef

define Build/Compile
	(cd $(PKG_BUILD_DIR); \
		GOOS=linux GOARCH=mipsle CGO_ENABLED=0 \
		go build -v -trimpath -tags "with_quic,with_utls" \
		-ldflags "-X 'github.com/sagernet/sing-box/constant.Version=$(PKG_VERSION)' -s -w -buildid=" \
		./cmd/sing-box; \
	)
endef

$(eval $(call BuildPackage,sing-box))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sing-box $(ROMFSDIR)/usr/bin/sing-box