include $(ROOTDIR)/rules.mk

PKG_NAME:=udns
PKG_VERSION:=0.4

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.corpit.ru/mjt/udns/
PKG_HASH:=115108dc791a2f9e99e150012bcb459d9095da2dd7d80699b584ac0ac3768710

PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk

$(eval $(call BuildPackage,udns))

romfs:
