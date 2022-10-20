SRC_NAME=nfs-utils-2.6.1
SRC_URL=https://www.kernel.org/pub/linux/utils/nfs-utils/2.6.1/$(SRC_NAME).tar.xz

CFLAGS +=$(COPT) -ffunction-sections -fdata-sections -fvisibility=hidden #`pkg-config --cflags uuid`
LDFLAGS += -Wl,--gc-sections #`pkg-config --libs uuid`

all: download_test extract_test config_test
	$(MAKE) -j$(HOST_NCPU) -C $(SRC_NAME)

download_test:
	( if [ ! -f $(SRC_NAME).tar.xz ]; then \
		wget -t5 --timeout=20 --no-check-certificate -O $(SRC_NAME).tar.xz $(SRC_URL); \
	fi )

extract_test:
	( if [ ! -d $(SRC_NAME) ]; then \
		tar xJf $(SRC_NAME).tar.xz; \
	fi )

config_test:
	( if [ -f ./config_done ]; then \
		echo "the same configuration"; \
	else \
		make configure && touch config_done; \
	fi )

configure:
	( cd $(SRC_NAME) && \
	autoreconf -fi ; \
	knfsd_cv_bsd_signals=no \
	PKG_CONFIG_PATH=$(STAGEDIR)/lib/pkgconfig \
	./configure \
		--prefix=/ \
		$(if $(CONFIG_IPV6),--enable-ipv6,--disable-ipv6) \
		--disable-mount \
		--disable-mountconfig \
		--disable-nfsdcltrack \
		--disable-nfsv4 \
		--disable-nfsv41 \
		--disable-uuid \
		--disable-gss \
		--enable-tirpc \
		--without-gssglue \
		--without-krb5 \
		--without-tcp-wrappers \
		LIBS="-ltirpc" \
		--host=$(HOST_TARGET) \
		--build=$(HOST_BUILD) ; \
	)

clean:
	if [ -f $(SRC_NAME)/Makefile ] ; then \
		$(MAKE) -C $(SRC_NAME) distclean ; \
	fi; \
	rm -f config_done

romfs:
	$(ROMFSINST) /usr/bin/nfsd.sh
	$(ROMFSINST) $(SRC_NAME)/utils/exportfs/exportfs /sbin/exportfs
	$(ROMFSINST) $(SRC_NAME)/utils/nfsd/nfsd /sbin/rpc.nfsd
	$(ROMFSINST) $(SRC_NAME)/utils/mountd/mountd /sbin/rpc.mountd
