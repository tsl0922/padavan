SRC_NAME = ZeroTierOne-1.10.6
SRC_URL = https://github.com/zerotier/ZeroTierOne/archive/refs/tags/1.10.6.tar.gz

THISDIR = $(shell pwd)
FLAGS := DEFS=""
CFLAGS := -Os -ffunction-sections -fdata-sections
LDFLAGS := -Wl,--gc-sections,--as-needed -static-libstdc++

all: download_test extract_test
	$(MAKE) -j$(HOST_NCPU) -C $(SRC_NAME)

download_test:
	( if [ ! -f $(SRC_NAME).tar.gz ]; then \
		wget -t5 --timeout=20 --no-check-certificate -O $(SRC_NAME).tar.gz $(SRC_URL); \
	fi )

extract_test:
	 if [ ! -d $(SRC_NAME) ] ; then \
		tar xf $(SRC_NAME).tar.gz ; \
		patch -d $(SRC_NAME) -p1 -i ../0007-add-cerrno-header-for-str-errno.patch ; \
		patch -d $(SRC_NAME) -p1 -i ../0008-fix-compilation-for-arm_cortex-a7-neon.patch; \
	 fi ;

clean:
	if [ -f $(SRC_NAME)/Makefile ] ; then \
		$(MAKE) -C $(SRC_NAME) distclean ; \
	fi ; \

romfs:
	$(ROMFSINST) $(THISDIR)/$(SRC_NAME)/zerotier-one /usr/bin/zerotier-one
	$(ROMFSINST) $(THISDIR)/zerotier.sh /usr/bin/zerotier.sh
	ln -sf zerotier-one $(ROMFSDIR)/usr/bin/zerotier-cli
	ln -sf zerotier-one $(ROMFSDIR)/usr/bin/zerotier-idtool
