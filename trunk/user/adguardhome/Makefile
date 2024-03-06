include $(ROOTDIR)/rules.mk

PKG_NAME:=AdGuardHome
PKG_VERSION:=0.107.34

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://github.com/AdguardTeam/AdGuardHome/archive/refs/tags/
PKG_SOURCE_URL_FILE:=v$(PKG_VERSION).tar.gz
PKG_HASH:=66a78f1817e5b465744259b21f2bfa2bdafbba1db6c65023293a98dc0ec3e754

include $(INCLUDE_DIR)/package.mk

LD_FLAGS:= \
	-X github.com/AdguardTeam/AdGuardHome/internal/version.channel=release \
	-X github.com/AdguardTeam/AdGuardHome/internal/version.version=$(PKG_VERSION)

define Build/Configure
	(cd $(PKG_BUILD_DIR); \
		NODE_OPTIONS=--openssl-legacy-provider make js-deps js-build; \
		zip -r embed.zip build; \
	)
endef

define Build/Compile
	(cd $(PKG_BUILD_DIR); \
		GOOS=linux GOARCH=mipsle CGO_ENABLED=0 \
		go build -v -o AdGuardHome \
		-trimpath -ldflags "$(LD_FLAGS) -s -w -buildid=" \
		.; \
	)
endef

$(eval $(call BuildPackage,adguardhome))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/usr/bin
	$(INSTALL_BIN) ./adguardhome.sh $(ROMFSDIR)/usr/bin/adguardhome.sh
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/AdGuardHome $(ROMFSDIR)/usr/bin/AdGuardHome
