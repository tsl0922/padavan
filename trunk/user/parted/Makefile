include $(ROOTDIR)/rules.mk

PKG_NAME:=parted
PKG_VERSION:=3.6

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=http://ftp.gnu.org/gnu/parted/
PKG_HASH:=3b43dbe33cca0f9a18601ebab56b7852b128ec1a3df3a9b30ccde5e73359e612

include $(INCLUDE_DIR)/package.mk

CONFIGURE_VARS += LIBS="-liconv"
CONFIGURE_ARGS += \
	--without-readline \
	--disable-shared \
	--disable-device-mapper \
	--disable-nls \
	--disable-debug

$(eval $(call BuildPackage,parted))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/parted/parted $(ROMFSDIR)/sbin/parted
