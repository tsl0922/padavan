include $(ROOTDIR)/rules.mk

PKG_NAME:=openssl
PKG_VERSION:=1.1.1w

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://www.openssl.org/source/
PKG_HASH:=cf3098950cb4d853ad95c0841f1f9c6d3dc102dccfcacd521d93925208b76ac8

include $(INCLUDE_DIR)/package.mk

OPENSSL_CIPHERS = no-capieng no-cms no-rfc3779 no-seed \
  no-ec2m no-err no-threads no-idea no-rc5 no-rmd160 \
  no-static-engine no-md2 no-sse2 no-dso no-ts no-mdc2 \
  no-cast no-whirlpool no-sctp no-srp no-ssl2 no-ssl3 \
  no-async no-blake2 no-hw-padlock no-aria no-hw \
  no-devcryptoeng no-ssl3-method no-tests no-sm2 \
  no-sm3 no-sm4 no-afalgeng no-comp

ifneq ($(CONFIG_FIRMWARE_INCLUDE_SSWAN),y)
OPENSSL_CIPHERS += no-rc2
endif

ifneq ($(CONFIG_FIRMWARE_INCLUDE_OPENSSL_EC),y)
OPENSSL_CIPHERS += no-ec
endif

CONFIGURE_CMD = ./Configure
CONFIGURE_ARGS = \
	linux-mips32 \
	shared \
	--prefix=$(STAGING_DIR) \
	--openssldir=/etc/ssl \
	$(OPENSSL_CIPHERS)

define Build/Install
	$(MAKE) -C $(PKG_BUILD_DIR)/$(MAKE_PATH) \
		$(MAKE_FLAGS) \
		DESTDIR= \
		install_sw
	$(MAKE) -C $(PKG_BUILD_DIR)/$(MAKE_PATH) \
		$(MAKE_FLAGS) \
		DESTDIR=$(STAGING_DIR) \
		install_ssldirs
endef

$(eval $(call BuildPackage,openssl))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/lib
	$(CP) $(PKG_INSTALL_DIR)/lib/libssl.so* $(ROMFSDIR)/lib/
	$(CP) $(PKG_INSTALL_DIR)/lib/libcrypto.so* $(ROMFSDIR)/lib/
	$(INSTALL_DIR) $(ROMFSDIR)/etc_ro
	$(INSTALL_DATA) ./openssl.cnf $(ROMFSDIR)/etc_ro/openssl.cnf
ifeq ($(CONFIG_FIRMWARE_INCLUDE_OPENSSL_EXE),y)
	$(INSTALL_DIR) $(ROMFSDIR)/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/bin/openssl $(ROMFSDIR)/bin/
endif

