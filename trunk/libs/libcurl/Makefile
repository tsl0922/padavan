include $(ROOTDIR)/rules.mk

PKG_NAME:=curl
PKG_VERSION:=8.1.2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=https://github.com/curl/curl/releases/download/curl-$(subst .,_,$(PKG_VERSION))/
PKG_HASH:=31b1118eb8bfd43cd95d9a3f146f814ff874f6ed3999b29d94f4d1e7dbac5ef6

include $(INCLUDE_DIR)/package.mk

TARGET_LDFLAGS += -Wl,-rpath-link=$(STAGEDIR)/lib

CONFIGURE_ARGS += \
	--enable-static \
	--enable-shared \
	--disable-gopher \
	--disable-ldap \
	--disable-ldaps \
	--disable-rtsp \
	--disable-telnet \
	--disable-tftp \
	--disable-smtp \
	--disable-pop3 \
	--disable-imap \
	--disable-sspi \
	--disable-dict \
	--disable-ares \
	--disable-debug \
	--disable-curldebug \
	--disable-manual \
	--disable-verbose \
	--disable-tls-srp \
	--disable-libcurl-option \
	--disable-threaded-resolver \
	--without-gnutls \
	--without-libidn \
	--without-libssh2 \
	--without-krb4 \
	--without-nss \
	--without-ca-path \
	--with-ca-bundle=/etc/ssl/certs/ca-certificates.crt \
	--with-random=/dev/urandom \
	--with-zlib=$(STAGING_DIR) \
	--with-ssl=$(STAGING_DIR)

define update_curl_config_prefix
	$(SED) 's,^\(prefix\|exec_prefix\)=.*,\1=$(STAGING_DIR)/usr,g' $(STAGING_DIR)/bin/curl-config
endef

Hooks/Install/Post += update_curl_config_prefix

$(eval $(call BuildPackage,curl))

romfs:
ifeq ($(CONFIG_FIRMWARE_INCLUDE_CURL),y)
	$(INSTALL_DIR) $(ROMFSDIR)/usr/bin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/bin/curl $(ROMFSDIR)/usr/bin/
endif
	$(INSTALL_DIR) $(ROMFSDIR)/lib
	$(CP) $(PKG_INSTALL_DIR)/lib/libcurl.so* $(ROMFSDIR)/lib/
