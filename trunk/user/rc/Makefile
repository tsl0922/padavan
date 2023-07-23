include $(ROOTDIR)/rules.mk

PKG_NAME:=rc

include $(INCLUDE_DIR)/package.mk

TARGET_CFLAGS += \
	-I$(STAGING_DIR)/include/shared \
	$(if $(STORAGE_ENABLED),-I$(STAGING_DIR)/include/libdisk,)

TARGET_LDFLAGS += -L$(STAGING_DIR)/lib

$(eval $(call BuildPackage,rc))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/rc $(ROMFSDIR)/sbin/rc
	$(LN) sbin/rc $(ROMFSDIR)/init
	$(LN) rc $(ROMFSDIR)/sbin/init
	$(LN) rc $(ROMFSDIR)/sbin/watchdog
	$(LN) rc $(ROMFSDIR)/sbin/gpio_btn
	$(LN) rc $(ROMFSDIR)/sbin/hotplug
	$(LN) rc $(ROMFSDIR)/sbin/shutdown
	$(LN) rc $(ROMFSDIR)/sbin/halt
	$(LN) rc $(ROMFSDIR)/sbin/reboot
	$(LN) rc $(ROMFSDIR)/sbin/ddns_updated
	$(LN) rc $(ROMFSDIR)/sbin/ntpc_updated
	$(LN) rc $(ROMFSDIR)/sbin/start_ddns
	$(LN) rc $(ROMFSDIR)/sbin/restart_dns
	$(LN) rc $(ROMFSDIR)/sbin/restart_dhcpd
	$(LN) rc $(ROMFSDIR)/sbin/restart_vpn_server
	$(LN) rc $(ROMFSDIR)/sbin/restart_vpn_client
	$(LN) rc $(ROMFSDIR)/sbin/run_telnetd
	$(LN) rc $(ROMFSDIR)/sbin/restart_firewall
	$(LN) rc $(ROMFSDIR)/sbin/pids
	$(LN) rc $(ROMFSDIR)/sbin/rstats
	$(LN) rc $(ROMFSDIR)/sbin/stop_wan
	$(LN) rc $(ROMFSDIR)/sbin/restart_wan
	$(LN) rc $(ROMFSDIR)/sbin/restart_networkmap
	$(LN) rc $(ROMFSDIR)/sbin/detect_link
	$(LN) rc $(ROMFSDIR)/sbin/detect_internet
	$(LN) rc $(ROMFSDIR)/sbin/detect_wan
	$(LN) rc $(ROMFSDIR)/sbin/reset_to_defaults
	$(LN) rc $(ROMFSDIR)/sbin/lan_eeprom_mac
	$(LN) rc $(ROMFSDIR)/sbin/wan_eeprom_mac
	$(LN) rc $(ROMFSDIR)/sbin/leds_front
	$(LN) rc $(ROMFSDIR)/sbin/leds_ether
	$(LN) rc $(ROMFSDIR)/sbin/radio2_restart
	$(LN) rc $(ROMFSDIR)/sbin/radio2_toggle
	$(LN) rc $(ROMFSDIR)/sbin/radio2_toggle_off
	$(LN) rc $(ROMFSDIR)/sbin/radio2_toggle_on
	$(LN) rc $(ROMFSDIR)/sbin/radio2_enable
	$(LN) rc $(ROMFSDIR)/sbin/radio2_disable
	$(LN) rc $(ROMFSDIR)/sbin/radio2_guest_enable
	$(LN) rc $(ROMFSDIR)/sbin/radio2_guest_disable
	$(LN) rc $(ROMFSDIR)/sbin/radio2_eeprom_mac
ifeq ($(BOARD_HAS_5G_RADIO),1)
	$(LN) rc $(ROMFSDIR)/sbin/radio5_restart
	$(LN) rc $(ROMFSDIR)/sbin/radio5_toggle
	$(LN) rc $(ROMFSDIR)/sbin/radio5_toggle_off
	$(LN) rc $(ROMFSDIR)/sbin/radio5_toggle_on
	$(LN) rc $(ROMFSDIR)/sbin/radio5_enable
	$(LN) rc $(ROMFSDIR)/sbin/radio5_disable
	$(LN) rc $(ROMFSDIR)/sbin/radio5_guest_enable
	$(LN) rc $(ROMFSDIR)/sbin/radio5_guest_disable
	$(LN) rc $(ROMFSDIR)/sbin/radio5_eeprom_mac
endif
ifdef CONFIG_RTL8367
	$(LN) rc $(ROMFSDIR)/sbin/rtl8367
endif
ifdef CONFIG_RAETH_ESW
	$(LN) rc $(ROMFSDIR)/sbin/mtk_esw
endif
ifdef CONFIG_RAETH
	$(LN) rc $(ROMFSDIR)/sbin/mtk_esw
endif
ifdef CONFIG_RT3352_INIC_MII
	$(LN) rc $(ROMFSDIR)/sbin/inicd
endif
ifeq ($(CONFIG_FIRMWARE_INCLUDE_OPENVPN),y)
	$(LN) rc $(ROMFSDIR)/sbin/ovpn_export_client
endif

ifeq ($(STORAGE_ENABLED),y)
ifeq ($(CONFIG_FIRMWARE_INCLUDE_SMBD),y)
	$(LN) rc $(ROMFSDIR)/sbin/run_samba
	$(LN) rc $(ROMFSDIR)/sbin/stop_samba
	$(LN) rc $(ROMFSDIR)/sbin/run_ftpsamba
	$(LN) rc $(ROMFSDIR)/sbin/stop_ftpsamba
endif
ifeq ($(CONFIG_FIRMWARE_INCLUDE_FTPD),y)
	$(LN) rc $(ROMFSDIR)/sbin/run_ftp
	$(LN) rc $(ROMFSDIR)/sbin/stop_ftp
ifneq ($(CONFIG_FIRMWARE_INCLUDE_SMBD),y)
	$(LN) rc $(ROMFSDIR)/sbin/run_ftpsamba
	$(LN) rc $(ROMFSDIR)/sbin/stop_ftpsamba
endif
endif
ifeq ($(CONFIG_FIRMWARE_INCLUDE_NFSD),y)
	$(LN) rc $(ROMFSDIR)/sbin/run_nfsd
	$(LN) rc $(ROMFSDIR)/sbin/stop_nfsd
endif
ifeq ($(CONFIG_FIRMWARE_INCLUDE_MINIDLNA),y)
	$(LN) rc $(ROMFSDIR)/sbin/run_minidlna
	$(LN) rc $(ROMFSDIR)/sbin/stop_minidlna
endif
ifeq ($(CONFIG_FIRMWARE_INCLUDE_FIREFLY),y)
	$(LN) rc $(ROMFSDIR)/sbin/run_firefly
	$(LN) rc $(ROMFSDIR)/sbin/stop_firefly
endif
ifeq ($(CONFIG_FIRMWARE_INCLUDE_TRANSMISSION),y)
	$(LN) rc $(ROMFSDIR)/sbin/run_transmission
	$(LN) rc $(ROMFSDIR)/sbin/stop_transmission
endif
ifeq ($(CONFIG_FIRMWARE_INCLUDE_ARIA),y)
	$(LN) rc $(ROMFSDIR)/sbin/run_aria
	$(LN) rc $(ROMFSDIR)/sbin/stop_aria
endif
endif
ifeq ($(STORAGE_ENABLED),y)
	$(LN) rc $(ROMFSDIR)/sbin/ejall
endif
ifdef CONFIG_ATA
	$(LN) rc $(ROMFSDIR)/sbin/ejata
endif
ifdef CONFIG_MMC_BLOCK
	$(LN) rc $(ROMFSDIR)/sbin/ejmmc
	$(LN) rc $(ROMFSDIR)/sbin/mdev_mmc
endif
ifdef CONFIG_BLK_DEV_SD
	$(LN) rc $(ROMFSDIR)/sbin/mdev_sd
endif
ifdef CONFIG_USB_SUPPORT
	$(LN) rc $(ROMFSDIR)/sbin/mdev_sr
	$(LN) rc $(ROMFSDIR)/sbin/mdev_sg
	$(LN) rc $(ROMFSDIR)/sbin/mdev_lp
	$(LN) rc $(ROMFSDIR)/sbin/mdev_net
	$(LN) rc $(ROMFSDIR)/sbin/mdev_tty
	$(LN) rc $(ROMFSDIR)/sbin/mdev_wdm
	$(LN) rc $(ROMFSDIR)/sbin/zerocd
	$(LN) rc $(ROMFSDIR)/sbin/usb5v
	$(LN) rc $(ROMFSDIR)/sbin/ejusb
	$(LN) rc $(ROMFSDIR)/sbin/ejusb1
ifneq ($(BOARD_NUM_USB_PORTS),1)
	$(LN) rc $(ROMFSDIR)/sbin/ejusb2
endif
endif