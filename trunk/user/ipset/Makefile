include $(ROOTDIR)/rules.mk

PKG_NAME:=ipset
PKG_VERSION:=7.11

PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk

SRC_DIR=./ipset-7.11
LINUX_DIR = $(ROOTDIR)/$(LINUXDIR)

CONFIGURE_ARGS += \
	--enable-static \
	--disable-shared \
	--with-kmod=yes \
	--with-kbuild=$(LINUX_DIR)

define Build/Compile
	$(MAKE) $(PKG_JOBS) -C $(PKG_BUILD_DIR) $(MAKE_FLAGS)
	$(MAKE) $(PKG_JOBS) -C $(PKG_BUILD_DIR) $(MAKE_FLAGS) modules
endef

$(eval $(call BuildPackage,ipset))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/src/ipset $(ROMFSDIR)/sbin/ipset
