include $(ROOTDIR)/rules.mk

PKG_NAME:=libid3tag
PKG_VERSION:=0.15.1b

include $(INCLUDE_DIR)/package.mk

SRC_DIR=./libid3tag-0.15.1b

CONFIGURE_ARGS += \
	--disable-debugging \
	--disable-profiling \
	$(if $(LIBS_BUILD_SHARED),--disable-static,--enable-static) \
	$(if $(LIBS_BUILD_SHARED),--enable-shared,--disable-shared)

$(eval $(call BuildPackage,libid3tag))

romfs:
ifeq ($(LIBS_BUILD_SHARED),y)
	$(INSTALL_DIR) $(ROMFSDIR)/lib
	$(CP) $(PKG_INSTALL_DIR)/lib/libid3tag.so* $(ROMFSDIR)/lib/
endif