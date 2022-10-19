SRC_NAME=openvpn-2.5.7
SRC_URL=https://github.com/OpenVPN/openvpn/archive/v2.5.7.tar.gz

CFLAGS += -ffunction-sections -fdata-sections
LDFLAGS += -Wl,--gc-sections

all: download_test extract_test config_test
	$(MAKE) -j$(HOST_NCPU) -C $(SRC_NAME)

download_test:
	( if [ ! -f $(SRC_NAME).tar.gz ]; then \
		wget -t5 --timeout=20 --no-check-certificate -O $(SRC_NAME).tar.gz $(SRC_URL); \
	fi )

extract_test:
	( if [ ! -d $(SRC_NAME) ]; then \
		tar xzf $(SRC_NAME).tar.gz; \
	fi )

config_test:
	( if [ -f ./config_done ]; then \
		echo "the same configuration"; \
	else \
		make configure && touch config_done; \
	fi )

configure:
	( cd $(SRC_NAME) ; \
	autoreconf -vi ; \
	IFCONFIG=/sbin/ifconfig \
	ROUTE=/sbin/route \
	NETSTAT=/bin/netstat \
	./configure \
		--prefix=/usr \
		--disable-debug \
		--disable-plugins \
		--disable-pkcs11 \
		--disable-ofb-cfb \
		--disable-selinux \
		--disable-systemd \
		--disable-iproute2 \
		--disable-unit-tests \
		--disable-management \
		--enable-small \
		--enable-multihome \
		--enable-pf \
		--enable-fragment \
		--enable-def-auth \
		--enable-lzo \
		--with-crypto-library=openssl \
		--with-sysroot=$(STAGEDIR) \
		--host=$(HOST_TARGET) \
		--build=$(HOST_BUILD) ; \
	)

clean:
	if [ -f $(SRC_NAME)/Makefile ] ; then \
		$(MAKE) -C $(SRC_NAME) distclean ; \
	fi ; \
	rm -f config_done

romfs:
	$(ROMFSINST) $(SRC_NAME)/src/openvpn/openvpn /usr/sbin/openvpn
	$(ROMFSINST) /usr/bin/openvpn-cert.sh
