include $(ROOTDIR)/rules.mk

PKG_NAME:=iptables
PKG_VERSION:=1.8.7

PKG_FIXUP:=autoreconf
PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

SRC_DIR=./iptables-1.8.7

CONFIGURE_ARGS += \
	--enable-static \
	--disable-shared \
	--disable-nftables \
	--disable-connlabel \
	--with-xtlibdir=/usr/lib/xtables \
	--with-kernel=$(ROOTDIR)/$(LINUXDIR) \
	--with-xt-lock-name="/var/lock/xtables.lock"

$(eval $(call BuildPackage,iptables))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/usr/lib/xtables
	$(INSTALL_DIR) $(ROMFSDIR)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/iptables/xtables-legacy-multi $(ROMFSDIR)/bin/xtables-multi
	$(LN) xtables-multi $(ROMFSDIR)/bin/iptables
	$(LN) xtables-multi $(ROMFSDIR)/bin/iptables-restore
	$(LN) xtables-multi $(ROMFSDIR)/bin/iptables-save
ifdef CONFIG_IPV6
	$(LN) xtables-multi $(ROMFSDIR)/bin/ip6tables
	$(LN) xtables-multi $(ROMFSDIR)/bin/ip6tables-restore
	$(LN) xtables-multi $(ROMFSDIR)/bin/ip6tables-save
endif
