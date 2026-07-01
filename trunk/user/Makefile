.EXPORT_ALL_VARIABLES:
.PHONY: all romfs clean

ifndef ROOTDIR
ROOTDIR=..
endif

ifndef ROMFSDIR
ROMFSDIR=$(ROOTDIR)/romfs
endif

UCLINUX_BUILD_USER=1
-include $(LINUX_CONFIG)
include $(PROJECT_CONFIG)
include $(ARCH_CONFIG)

CFLAGS  += -ffunction-sections -fdata-sections
CPPFLAGS += -ffunction-sections -fdata-sections
LDFLAGS += -Wl,--gc-sections

INSTALLDIR = $(ROOTDIR)/romfs

BOARD_CONFIG = $(ROOTDIR)/user/boards.mk
CFLAGS_CONFIG = $(ROOTDIR)/user/cflags.mk

DROPBEAR_ENABLED=n
FRP_ENABLE=n
FS_EXT_ENABLED=n
FS_FAT_ENABLED=n
FS_NTFS_ENABLED=n
OPENSSH_ENABLED=n
SAMBA_ENABLED=n
SHADOWSOCKS_ENABLE=n
SOFTETHERVPN_ENABLE=n
STORAGE_ENABLED=n
TROJAN_ENABLE=n

ifneq ($(CONFIG_FIRMWARE_INCLUDE_OPENSSH),y)
ifeq ($(CONFIG_FIRMWARE_INCLUDE_DROPBEAR),y)
DROPBEAR_ENABLED=y
ifeq ($(CONFIG_FIRMWARE_INCLUDE_SFTP),y)
OPENSSH_ENABLED=y
endif
endif
else
OPENSSH_ENABLED=y
endif

ifeq ($(CONFIG_FIRMWARE_INCLUDE_WINS),y)
SAMBA_ENABLED=y
endif

ifdef CONFIG_BLK_DEV_SD
STORAGE_ENABLED=y
endif

ifdef CONFIG_MMC_BLOCK
STORAGE_ENABLED=y
endif

ifeq ($(STORAGE_ENABLED),y)
ifeq ($(CONFIG_FIRMWARE_ENABLE_EXT2),y)
FS_EXT_ENABLED=y
endif
ifeq ($(CONFIG_FIRMWARE_ENABLE_EXT3),y)
FS_EXT_ENABLED=y
endif
ifeq ($(CONFIG_FIRMWARE_ENABLE_EXT4),y)
FS_EXT_ENABLED=y
endif
ifeq ($(CONFIG_FIRMWARE_ENABLE_FAT),y)
FS_FAT_ENABLED=y
endif
ifeq ($(CONFIG_FIRMWARE_ENABLE_EXFAT),y)
FS_FAT_ENABLED=y
endif
ifeq ($(CONFIG_FIRMWARE_INCLUDE_NTFS_3G),y)
FS_NTFS_ENABLED=y
endif
ifeq ($(CONFIG_FIRMWARE_INCLUDE_SMBD),y)
SAMBA_ENABLED=y
endif
endif

ifneq ($(filter y,$(CONFIG_FIRMWARE_INCLUDE_SOFTETHERVPN_CMD) $(CONFIG_FIRMWARE_INCLUDE_SOFTETHERVPN_CLIENT) $(CONFIG_FIRMWARE_INCLUDE_SOFTETHERVPN_SERVER)),)
SOFTETHERVPN_ENABLE=y
endif

ifneq ($(filter y,$(CONFIG_FIRMWARE_INCLUDE_SSSERVER) $(CONFIG_FIRMWARE_INCLUDE_SHADOWSOCKS)),)
SHADOWSOCKS_ENABLE=y
endif

ifneq ($(filter y,$(CONFIG_FIRMWARE_INCLUDE_FRPC) $(CONFIG_FIRMWARE_INCLUDE_FRPS)),)
FRP_ENABLE=y
endif

ifneq ($(filter y,$(CONFIG_FIRMWARE_INCLUDE_WING) $(CONFIG_FIRMWARE_INCLUDE_TROJAN)),)
TROJAN_ENABLE=y
endif

dir_y =
dir_n =
dir_  =
dir_y                      			+= ralinkiappd
dir_y						+= busybox
dir_y						+= iptables
dir_y						+= coremark
ifneq ($(CONFIG_WITHOUT_KERNEL),y)
dir_$(CONFIG_FIRMWARE_INCLUDE_IPSET)		+= ipset
endif
dir_y						+= ebtables
dir_y						+= iproute2
dir_y						+= wireless-tools
dir_y						+= shared
dir_$(STORAGE_ENABLED)				+= libdisk
dir_y						+= nvram
dir_y						+= mtd_write
dir_y						+= networkmap
dir_y						+= httpd
dir_y						+= www
dir_y						+= rc
dir_$(CONFIG_FIRMWARE_INCLUDE_ANTFS)		+= antfs
dir_y						+= infosvr
dir_y						+= inadyn
dir_y						+= miniupnpd
dir_y						+= dnsmasq
dir_y						+= igmpproxy
ifneq ($(CONFIG_FIRMWARE_INCLUDE_MSD_LITE),y)
dir_y						+= udpxy
endif
dir_y						+= utils
dir_y						+= lanauth
dir_y						+= 802.1x
dir_y						+= wpa_supplicant
dir_y						+= pppd
dir_y						+= pppoe
dir_y						+= pppoe-relay
dir_y						+= poptop
dir_y						+= xl2tpd
dir_$(CONFIG_FIRMWARE_INCLUDE_RPL2TP)		+= rp-l2tp
dir_y						+= lldt
dir_y						+= scripts
dir_$(FRP_ENABLE)				+= frp
dir_$(SAMBA_ENABLED)				+= samba36
dir_$(SAMBA_ENABLED)				+= wsdd2
dir_$(SOFTETHERVPN_ENABLE)			+= softethervpn
dir_$(OPENSSH_ENABLED)				+= openssh
dir_$(DROPBEAR_ENABLED)				+= dropbear
dir_$(CONFIG_FIRMWARE_INCLUDE_3PROXY)		+= 3proxy
dir_$(CONFIG_FIRMWARE_INCLUDE_ADBYBY)		+= adbyby
dir_$(CONFIG_FIRMWARE_INCLUDE_ADGUARDHOME)	+= adguardhome
dir_$(CONFIG_FIRMWARE_INCLUDE_ALIDDNS)		+= aliddns
dir_$(CONFIG_FIRMWARE_INCLUDE_ALDRIVER)		+= aliyundrive-webdav
dir_$(CONFIG_FIRMWARE_INCLUDE_DDNSTO)		+= ddnsto
dir_$(CONFIG_FIRMWARE_INCLUDE_DNSFORWARDER)	+= dns-forwarder
dir_$(CONFIG_FIRMWARE_INCLUDE_DOGCOM)		+= dogcom
dir_$(CONFIG_FIRMWARE_INCLUDE_GDUT_DRCOM)	+= gdut-drcom
dir_$(CONFIG_FIRMWARE_INCLUDE_HTOP)		+= htop
dir_$(CONFIG_FIRMWARE_INCLUDE_IPERF3)		+= iperf3
dir_$(CONFIG_FIRMWARE_INCLUDE_LRZSZ)		+= lrzsz
dir_$(CONFIG_FIRMWARE_INCLUDE_MTR)		+= mtr
dir_$(CONFIG_FIRMWARE_INCLUDE_MENTOHUST)	+= mentohust
dir_$(CONFIG_FIRMWARE_INCLUDE_MICROSOCKS)	+= microsocks
dir_$(CONFIG_FIRMWARE_INCLUDE_MINIEAP)		+= minieap
dir_$(CONFIG_FIRMWARE_INCLUDE_MSD_LITE)		+= msd_lite
dir_$(CONFIG_FIRMWARE_INCLUDE_NANO)		+= nano
dir_$(CONFIG_FIRMWARE_INCLUDE_NJIT_CLIENT)	+= njit-client
dir_$(CONFIG_FIRMWARE_INCLUDE_OPENVPN)		+= openvpn
dir_$(CONFIG_FIRMWARE_INCLUDE_REDSOCKS)		+= redsocks
dir_$(CONFIG_FIRMWARE_INCLUDE_SCUTCLIENT)	+= scutclient
dir_$(CONFIG_FIRMWARE_INCLUDE_SMARTDNS)		+= smartdns
dir_$(CONFIG_FIRMWARE_INCLUDE_SOCAT)		+= socat
dir_$(CONFIG_FIRMWARE_INCLUDE_SQM)		+= sqm-qos
dir_$(CONFIG_FIRMWARE_INCLUDE_SRELAY)		+= srelay
dir_$(CONFIG_FIRMWARE_INCLUDE_SSWAN)		+= strongswan
dir_$(CONFIG_FIRMWARE_INCLUDE_TCPDUMP)		+= tcpdump
dir_$(CONFIG_FIRMWARE_INCLUDE_TTYD)		+= ttyd
dir_$(CONFIG_FIRMWARE_INCLUDE_TUNSAFE)		+= tunsafe
dir_$(CONFIG_FIRMWARE_INCLUDE_VLMCSD)		+= vlmcsd
dir_$(CONFIG_FIRMWARE_INCLUDE_xTUN) 		+= xTun
dir_$(CONFIG_FIRMWARE_INCLUDE_XUPNPD)		+= xupnpd
dir_$(CONFIG_FIRMWARE_INCLUDE_ZEROTIER)		+= zerotier

dir_$(SHADOWSOCKS_ENABLE)			+= shadowsocks
ifeq ($(CONFIG_FIRMWARE_INCLUDE_SHADOWSOCKS),y)
dir_$(CONFIG_FIRMWARE_INCLUDE_NAIVEPROXY)	+= naiveproxy
dir_$(CONFIG_FIRMWARE_INCLUDE_TROJAN)	        += trojan
dir_$(CONFIG_FIRMWARE_INCLUDE_V2RAY)	        += v2ray
dir_$(CONFIG_FIRMWARE_INCLUDE_XRAY)	        += xray
dir_$(CONFIG_FIRMWARE_INCLUDE_SSOBFS)		+= simple-obfs
dir_$(CONFIG_FIRMWARE_INCLUDE_SINGBOX)	        += sing-box
endif
dir_$(CONFIG_FIRMWARE_INCLUDE_SHADOWSOCKS)	+= chinadns-ng
dir_$(CONFIG_FIRMWARE_INCLUDE_SHADOWSOCKS)	+= chnroute
dir_$(CONFIG_FIRMWARE_INCLUDE_SHADOWSOCKS)	+= dns2tcp
dir_$(CONFIG_FIRMWARE_INCLUDE_SHADOWSOCKS)	+= dnsproxy
dir_$(CONFIG_FIRMWARE_INCLUDE_SHADOWSOCKS)	+= ipt2socks
dir_$(CONFIG_FIRMWARE_INCLUDE_SHADOWSOCKS)	+= lua51
dir_$(CONFIG_FIRMWARE_INCLUDE_SHADOWSOCKS)	+= resolveip
dir_$(CONFIG_FIRMWARE_INCLUDE_SHADOWSOCKS)	+= skipdbv2

ifeq ($(CONFIG_FIRMWARE_INCLUDE_WING),y)
dir_y 						+= wing
dir_y 						+= ipt2socks
endif

ifeq ($(CONFIG_FIRMWARE_INCLUDE_WIREGUARD),y)
dir_y 						+= wireguard
dir_y 						+= wireguard-tools
dir_y						+= wg-quick
endif

ifdef CONFIG_BLK_DEV_SD
dir_$(CONFIG_FIRMWARE_INCLUDE_HDPARM)		+= hdparm
endif

ifdef CONFIG_USB_SUPPORT
dir_y						+= comgt-0.32
dir_y						+= p910nd
dir_y						+= uqmi
dir_y						+= usb-modeswitch
dir_$(CONFIG_FIRMWARE_INCLUDE_DUMP1090)		+= dump1090
dir_$(CONFIG_FIRMWARE_INCLUDE_LPRD)		+= LPRng
dir_$(CONFIG_FIRMWARE_INCLUDE_U2EC)		+= u2ec
endif

ifeq ($(STORAGE_ENABLED),y)
dir_y						+= util-linux
dir_$(CONFIG_FIRMWARE_INCLUDE_NFSD)		+= nfs-utils
ifneq (,$(filter y,$(CONFIG_FIRMWARE_INCLUDE_NFSD) $(CONFIG_FIRMWARE_INCLUDE_NFSC)))
dir_y						+= rpcbind
endif
dir_y						+= optware
dir_y						+= util-linux
dir_$(FS_FAT_ENABLED)				+= dosfstools
dir_$(FS_EXT_ENABLED)				+= e2fsprogs
dir_$(FS_NTFS_ENABLED)				+= ntfs-3g
dir_$(CONFIG_FIRMWARE_INCLUDE_ARIA)		+= aria2
dir_$(CONFIG_FIRMWARE_INCLUDE_FIREFLY)		+= firefly
dir_$(CONFIG_FIRMWARE_INCLUDE_MINIDLNA)		+= minidlna
dir_$(CONFIG_FIRMWARE_INCLUDE_PARTED)		+= parted
dir_$(CONFIG_FIRMWARE_INCLUDE_FTPD)		+= vsftpd
dir_$(CONFIG_FIRMWARE_INCLUDE_TRANSMISSION)	+= transmission
endif

ifdef CONFIG_MTD_UBI
ifneq ($(STORAGE_ENABLED),y)
dir_y						+= optware
dir_y						+= util-linux
endif
dir_y						+= mtd-utils
endif

all: $(patsubst %,%_only,$(dir_y))

romfs: $(patsubst %,%_romfs,$(dir_y))

clean: $(patsubst %,%_clean,$(dir_y) $(dir_n) $(dir_))

%_only:
	@echo "\033[1;30;43m BUILD \033[0m \033[1;30;44m $(@:_only=) \033[0m"
	$(MAKE) -C $(@:_only=)

%_romfs:
	@echo "\033[1;30;42m ROMFS \033[0m \033[1;30;44m $(@:_romfs=) \033[0m"
	$(MAKE) -C $(@:_romfs=) romfs

%_clean:
	@echo "\033[1;30;42m CLEAN \033[0m \033[1;30;44m $(@:_clean=) \033[0m"
	$(MAKE) -C $(@:_clean=) clean || true
