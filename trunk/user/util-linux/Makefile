include $(ROOTDIR)/rules.mk

PKG_NAME:=util-linux
PKG_VERSION:=2.39.4

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.xz
PKG_SOURCE_URL:=@KERNEL/linux/utils/$(PKG_NAME)/v2.39
PKG_HASH:=6c4f8723dafd41c39d93ecbf16509fc88c33cd5bd3277880ae5a1d97a014fd0e

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

CONFIGURE_ARGS += \
	--without-tinfo \
	--without-ncurses \
	--without-user \
	--without-cap-ng \
	--without-util \
	--without-ncursesw \
	--without-slang \
	--without-selinux \
	--without-audit \
	--without-udev \
	--without-python \
	--without-systemd \
	--without-btrfs \
	--without-readline \
	--enable-libblkid \
	--enable-blkid \
	--enable-libuuid \
	--disable-cal \
	--disable-fdformat \
	--disable-hwclock \
	--disable-bfs \
	--disable-minix \
	--disable-ipcrm \
	--disable-ipcs \
	--disable-setterm \
	--disable-nsenter \
	--disable-rfkill \
	--disable-setpriv \
	--disable-logger \
	--disable-lslogins \
	--disable-runuser \
	--disable-libsmartcols \
	--disable-raw \
	--disable-ul \
	--disable-chfn-chsh \
	--disable-last \
	--disable-fdisks \
	--disable-libfdisk \
	--disable-zramctl \
	--disable-mesg \
	--disable-nologin \
	--disable-more \
	--disable-chmem \
	--disable-lsmem \
	--disable-bash-completion \
	--disable-nls \
	--disable-libmount \
	--disable-mount \
	--disable-losetup \
	--disable-fsck \
	--disable-partx \
	--disable-uuidd \
	--disable-mountpoint \
	--disable-fallocate \
	--disable-unshare \
	--disable-eject \
	--disable-agetty \
	--disable-cramfs \
	--disable-wdctl \
	--disable-switch_root \
	--disable-pivot_root \
	--disable-kill \
	--disable-utmpdump \
	--disable-rename \
	--disable-login \
	--disable-sulogin \
	--disable-su \
	--disable-schedutils \
	--disable-wall \
	--disable-use-tty-group \
	--disable-makeinstall-chown \
	--disable-makeinstall-setuid
CONFIGURE_VARS += scanf_cv_type_modifier=no

$(eval $(call BuildPackage,util-linux))

romfs:
	$(INSTALL_DIR) $(ROMFSDIR)/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/sbin/blkid $(ROMFSDIR)/sbin/blkid
	$(INSTALL_DIR) $(ROMFSDIR)/lib
	$(CP) $(PKG_INSTALL_DIR)/lib/libblkid.so* $(ROMFSDIR)/lib/
	$(CP) $(PKG_INSTALL_DIR)/lib/libuuid.so* $(ROMFSDIR)/lib/

