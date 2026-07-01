include $(ROOTDIR)/rules.mk

PKG_NAME:=udns
PKG_VERSION:=0.6

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.corpit.ru/mjt/udns/
PKG_HASH:=696a2d0d518da985d975a65e11d166f3f57cdbd1d42376a0b85307f49601c6e8

PKG_FIXUP:=autoreconf

include $(INCLUDE_DIR)/package.mk

$(eval $(call BuildPackage,udns))

romfs:
