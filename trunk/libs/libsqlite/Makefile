include $(ROOTDIR)/rules.mk

PKG_NAME:=sqlite
PKG_VERSION:=3490200

PKG_SOURCE:=$(PKG_NAME)-autoconf-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://www.sqlite.org/2025/
PKG_HASH:=5c6d8697e8a32a1512a9be5ad2b2e7a891241c334f56f8b0fb4fc6051e1652e8

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-autoconf-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

CONFIGURE_ARGS += \
	$(if $(LIBS_BUILD_SHARED),--disable-static,--enable-static) \
	$(if $(LIBS_BUILD_SHARED),--enable-shared,--disable-shared) \
	--disable-readline \
	--disable-dynamic-extensions \
	--disable-fts3 \
	--disable-fts4 \
	--disable-fts5 \
	--disable-json1 \
	--disable-rtree \
	--disable-session \
	--disable-debug \
	--disable-static-shell

$(eval $(call BuildPackage,sqlite))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/lib
	$(CP) $(PKG_INSTALL_DIR)/lib/libsqlite3.so* $(ROMFSDIR)/lib/
