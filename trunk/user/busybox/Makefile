include $(ROOTDIR)/rules.mk

PKG_NAME:=busybox
PKG_VERSION:=1.24

include $(INCLUDE_DIR)/package.mk

SRC_DIR=./busybox-1.24.x

$(eval $(call BuildPackage,busybox))

romfs:
	$(MAKE) -C $(PKG_BUILD_DIR) \
		$(MAKE_FLAGS) \
		CONFIG_PREFIX=$(ROMFSDIR) \
		install
