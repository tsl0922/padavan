include $(ROOTDIR)/rules.mk

PKG_NAME:=tcpdump
PKG_VERSION:=4.99.5

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://www.tcpdump.org/release/
PKG_HASH:=8c75856e00addeeadf70dad67c9ff3dd368536b2b8563abf6854d7c764cd3adb

include $(INCLUDE_DIR)/package.mk

CONFIGURE_VARS += \
	ac_cv_linux_vers=3 \
	ac_cv_header_rpc_rpcent_h=no \
	ac_cv_lib_rpc_main=no \
	ac_cv_path_PCAP_CONFIG="$(STAGING_DIR)/bin/pcap-config" \
	td_cv_buggygetaddrinfo=no

CONFIGURE_ARGS += \
	--without-crypto \
	--without-smi \
	--without-cap-ng \
	--disable-smb \
	$(if $(CONFIG_IPV6),--enable-ipv6,--disable-ipv6)

TARGET_CFLAGS += -DTCPDUMP_MINI
MAKE_FLAGS += TCPDUMP_MINI=1

$(eval $(call BuildPackage,tcpdump))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/tcpdump $(ROMFSDIR)/usr/sbin/tcpdump
