include $(ROOTDIR)/rules.mk

PKG_NAME:=utils

include $(INCLUDE_DIR)/package.mk

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)/cpu_usage $(MAKE_FLAGS) all
	$(MAKE) -C $(PKG_BUILD_DIR)/qdma $(MAKE_FLAGS) all
	$(MAKE) -C $(PKG_BUILD_DIR)/switch $(MAKE_FLAGS) all
ifdef CONFIG_RA_HW_NAT
	$(MAKE) -C $(PKG_BUILD_DIR)/hw_nat $(MAKE_FLAGS) all
endif
endef

$(eval $(call BuildPackage,utils))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/cpu_usage/cpu $(ROMFSDIR)/usr/bin/cpu
	$(INSTALL_DIR) $(ROMFSDIR)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/qdma/qdma $(ROMFSDIR)/sbin/qdma
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/switch/switch $(ROMFSDIR)/sbin/switch
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/switch/mii_mgr $(ROMFSDIR)/sbin/mii_mgr
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/switch/mii_mgr_cl45 $(ROMFSDIR)/sbin/mii_mgr_cl45
ifdef CONFIG_RA_HW_NAT
	$(INSTALL_DIR) $(ROMFSDIR)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/hw_nat/hw_nat $(ROMFSDIR)/bin/hw_nat
endif
