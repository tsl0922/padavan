include $(ROOTDIR)/rules.mk

PKG_NAME:=pcre
PKG_VERSION:=8.45

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).zip
PKG_SOURCE_URL:=@SF/pcre
PKG_HASH:=5b709aa45ea3b8bb73052947200ad187f651a2049158fb5bbfed329e4322a977

include $(INCLUDE_DIR)/package.mk

CONFIGURE_ARGS += \
	--enable-utf8 \
	--enable-unicode-properties \
	--enable-pcre16 \
	--disable-cpp \
	--with-match-limit-recursion=16000

define update_pcre_config_prefix
	$(SED) 's,^\(prefix\|exec_prefix\)=.*,\1=$(STAGING_DIR)/usr,g' $(STAGING_DIR)/bin/pcre-config
endef

Hooks/Install/Post += update_pcre_config_prefix

$(eval $(call BuildPackage,pcre))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/lib
	$(CP) $(PKG_INSTALL_DIR)/lib/libpcre.so* $(ROMFSDIR)/lib/
