SRC_NAME=rpcbind-1.2.6

CFLAGS += -fvisibility=hidden

all: config_test
	$(MAKE) -j$(HOST_NCPU) -C $(SRC_NAME)

config_test:
	( if [ -f ./config_done ]; then \
		echo "the same configuration"; \
	else \
		make configure && touch config_done; \
	fi )

configure:
	( cd $(SRC_NAME) ; \
	autoreconf -fi ; \
	./configure \
		--prefix=/ \
		$(if $(CONFIG_IPV6),--enable-ipv6,--disable-ipv6) \
		--disable-libwrap \
		--disable-debug \
		--disable-warmstarts \
		--with-rpcuser=nobody \
		--without-nss-modules \
		--without-systemdsystemunitdir \
		--host=$(HOST_TARGET) \
		--build=$(HOST_BUILD); \
	)

clean:
	if [ -f $(SRC_NAME)/Makefile ] ; then \
		$(MAKE) -C $(SRC_NAME) distclean ; \
	fi ; \
	rm -rf config_done configs

romfs:
	$(ROMFSINST) $(SRC_NAME)/rpcbind /sbin/rpcbind
