include $(ROOTDIR)/rules.mk

PKG_NAME:=readline
PKG_VERSION:=7.0

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@GNU/readline
PKG_HASH:=750d437185286f40a369e1e4f4764eda932b9459b5ec9a731628393dd3d32334

include $(INCLUDE_DIR)/package.mk

$(eval $(call BuildPackage,readline))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/lib
	$(CP) $(PKG_INSTALL_DIR)/lib/libreadline.so* $(ROMFSDIR)/lib/
