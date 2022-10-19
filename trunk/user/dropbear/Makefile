SRC_NAME=dropbear-2022.82
SRC_URL=https://matt.ucc.asn.au/dropbear/releases/$(SRC_NAME).tar.bz2

ifeq ($(CONFIG_FIRMWARE_INCLUDE_SFTP),y)
COPT=-DSFTPSERVER_PATH=\"/usr/libexec/sftp-server\"
else
COPT=-DSFTPSERVER_PATH=\"/opt/libexec/sftp-server\"
endif
ifneq ($(CONFIG_FIRMWARE_INCLUDE_DROPBEAR_FAST_CODE),y)
COPT+=-DDROPBEAR_SMALL_CODE=1
else
COPT+=-DDROPBEAR_SMALL_CODE=0
endif
CFLAGS+=$(COPT) -ffunction-sections -fdata-sections
LDFLAGS += -Wl,--gc-sections

all: download_test extract_test config_test
	$(MAKE) -j$(HOST_NCPU) -C $(SRC_NAME) PROGRAMS="dropbear dbclient dropbearkey dropbearconvert scp" MULTI=1

download_test:
	( if [ ! -f $(SRC_NAME).tar.bz2 ]; then \
		wget -t5 --timeout=20 --no-check-certificate -O $(SRC_NAME).tar.bz2 $(SRC_URL); \
	fi )

extract_test:
	( if [ ! -d $(SRC_NAME) ]; then \
		tar xjf $(SRC_NAME).tar.bz2; \
	fi )

config_test:
	( if [ -f ./config_done ]; then \
		echo "the same configuration"; \
	else \
		make configure && touch config_done; \
	fi )

configure:
	( cd $(SRC_NAME) ; \
	autoreconf -v; \
	./configure \
		--prefix=/usr \
		--disable-harden \
		--disable-lastlog \
		--disable-utmp \
		--disable-utmpx \
		--disable-wtmp \
		--disable-wtmpx \
		--disable-loginfunc \
		--disable-pututline \
		--disable-pututxline \
		--enable-zlib \
		--enable-bundled-libtom \
		--host=$(HOST_TARGET) \
		--build=$(HOST_BUILD) ; \
	)

clean:
	if [ -f $(SRC_NAME)/Makefile ] ; then \
		$(MAKE) -C $(SRC_NAME) distclean ; \
	fi ; \
	rm -f dropbearmulti
	rm -f config_done

romfs:
	cp $(SRC_NAME)/dropbearmulti .
	$(STRIP) dropbearmulti
	$(ROMFSINST) /usr/bin/dropbearmulti
	$(ROMFSINST) -p +x /usr/bin/sshd.sh
	$(ROMFSINST) -s dropbearmulti /usr/bin/ssh
	$(ROMFSINST) -s dropbearmulti /usr/bin/scp
	$(ROMFSINST) -s dropbearmulti /usr/bin/dropbearkey
	$(ROMFSINST) -s dropbearmulti /usr/bin/dropbearconvert
	$(ROMFSINST) -s /usr/bin/dropbearmulti /usr/sbin/dropbear
