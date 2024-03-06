SRC_NAME=minidlna-1.3.3
SRC_URL=https://sourceforge.net/projects/minidlna/files/minidlna/1.3.3/minidlna-1.3.3.tar.gz 

CFLAGS  += -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64

THISDIR = $(shell pwd)

all: download_test extract_test config_test
	$(MAKE) -j$(HOST_NCPU) -C $(SRC_NAME)

download_test:
	( if [ ! -f $(SRC_NAME).tar.gz ]; then \
		wget -t5 --timeout=20 --no-check-certificate -O $(SRC_NAME).tar.gz $(SRC_URL); \
	fi )

extract_test:
	( if [ ! -d $(SRC_NAME) ]; then \
		tar xzf $(SRC_NAME).tar.gz; \
		patch -d $(SRC_NAME) -p1 -i ../libav-codec-id.diff; \
	fi )

config_test:
	( if [ -f ./config_done ]; then \
		echo "the same configuration"; \
	else \
		make configure && touch config_done; \
	fi )

configure:
	( cd $(SRC_NAME) ; \
	./autogen.sh; \
	PKG_CONFIG_PATH=$(STAGEDIR)/lib/pkgconfig \
	./configure \
		--prefix=/usr \
		--disable-nls \
		--enable-asus \
		--enable-tivo \
		--with-libiconv-prefix=$(STAGEDIR) \
		--with-log-path=/mnt/minidlna \
		--with-db-path=/mnt/minidlna \
		--with-os-name="Linux" \
		--with-os-version="Linux/Cross_compiled" \
		--with-os-url="http://www.kernel.org/" \
		--host=$(HOST_TARGET) \
		--build=$(HOST_BUILD) ; \
	)

clean:
	rm -f config_done
	rm -rf $(SRC_NAME)

romfs:
	$(ROMFSINST) $(THISDIR)/$(SRC_NAME)/minidlnad /usr/bin/minidlnad

