include $(ROOTDIR)/rules.mk

PKG_NAME:=curl
PKG_VERSION:=8.4.0

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=https://github.com/curl/curl/releases/download/curl-$(subst .,_,$(PKG_VERSION))/
PKG_HASH:=16c62a9c4af0f703d28bda6d7bbf37ba47055ad3414d70dec63e2e6336f2a82d

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
