# source from http://osp.avm.de/fritzbox/fritzbox-7560/source-files-FRITZ.Box_7560-07.11.tar
# 07.19  from http://osp.avm.de/fritzbox/fritzbox-7590/source-files-FRITZ.Box_7590-07.19.tar.gz
include $(ROOTDIR)/rules.mk

PKG_NAME:=antfs
PKG_VERSION:=07.19

include $(INCLUDE_DIR)/package.mk

SRC_DIR=./antfs-07.19

MODULE_CONFIG := CONFIG_ANTFS_FS=m
MODULE_CONFIG += CONFIG_ANTFS_SYMLINKS=y

define Build/Compile
	$(MAKE) $(PKG_JOBS) -C $(ROOTDIR)/$(LINUXDIR) \
		M=$(PKG_BUILD_DIR) \
		$(MAKE_FLAGS) LDFLAGS= \
		modules $(MODULE_CONFIG)
endef

$(eval $(call BuildPackage,antfs))

romfs:
