include $(ROOTDIR)/rules.mk

PKG_NAME:=ncurses
PKG_VERSION:=6.4

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@GNU/$(PKG_NAME)
PKG_HASH:=6931283d9ac87c5073f30b6290c4c75f21632bb4fc3603ac8100812bed248159

include $(INCLUDE_DIR)/package.mk

CONFIGURE_ARGS += \
	--with-terminfo-dirs=/usr/share/terminfo \
	--with-default-terminfo-dir=/usr/share/terminfo \
	--with-pkg-config-libdir=/lib/pkgconfig \
	--without-ada \
	--without-termlib \
	--without-progs \
	--without-tests \
	--without-manpages \
	--enable-widec \
	--with-shared \
	--without-debug \
	--with-normal \
	--without-profile \
	--enable-overwrite \
	--enable-pc-files

define update_ncurses_config_prefix
	$(SED) 's,^\(prefix\|exec_prefix\)=.*,\1=$(STAGING_DIR)/usr,g' $(STAGING_DIR)/bin/ncursesw6-config
endef

Hooks/Install/Post += update_ncurses_config_prefix

$(eval $(call BuildPackage,ncurses))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/lib
	$(CP) $(PKG_INSTALL_DIR)/lib/libncursesw.so* $(ROMFSDIR)/lib/
