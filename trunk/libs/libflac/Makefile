include $(ROOTDIR)/rules.mk

PKG_NAME:=flac
PKG_VERSION:=1.4.3

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=https://downloads.xiph.org/releases/flac/
PKG_HASH:=6c58e69cd22348f441b861092b825e591d0b822e106de6eb0ee4d05d27205b70

include $(INCLUDE_DIR)/package.mk

CONFIGURE_ARGS += \
	$(if $(LIBS_BUILD_SHARED),--disable-static,--enable-static) \
	$(if $(LIBS_BUILD_SHARED),--enable-shared,--disable-shared) \
	--disable-cpplibs \
	--disable-sse \
	--disable-3dnow \
	--disable-altivec \
	--disable-doxgen-docs \
	--disable-local-xmms-plugin \
	--disable-xmms-plugin \
	--disable-ogg \
	--disable-oggtest \
	--disable-debug

$(eval $(call BuildPackage,flac))

romfs:
ifeq ($(LIBS_BUILD_SHARED),y)
	$(INSTALL_DIR) $(ROMFSDIR)/lib
	$(CP) $(PKG_INSTALL_DIR)/lib/libFLAC.so* $(ROMFSDIR)/lib/
endif
