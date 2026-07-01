include $(ROOTDIR)/rules.mk

PKG_NAME:=readline
PKG_VERSION:=8.2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@GNU/readline
PKG_HASH:=3feb7171f16a84ee82ca18a36d7b9be109a52c04f492a053331d7d1095007c35

include $(INCLUDE_DIR)/package.mk

$(eval $(call BuildPackage,readline))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/lib
	$(CP) $(PKG_INSTALL_DIR)/lib/libreadline.so* $(ROMFSDIR)/lib/
