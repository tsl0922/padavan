include $(ROOTDIR)/rules.mk

PKG_NAME:=lzo
PKG_VERSION:=2.10

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=http://www.oberhumer.com/opensource/lzo/download/
PKG_HASH:=c0f892943208266f9b6543b3ae308fab6284c5c90e627931446fb49b4221a072

CMAKE_BINARY_SUBDIR:=build

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

CMAKE_OPTIONS += \
	-DENABLE_STATIC=ON \
	-DENABLE_SHARED=OFF \
	-DBUILD_TESTING=OFF

$(eval $(call BuildPackage,lzo))

romfs:
