SRC_NAME=json-c-0.16
SRC_URL=https://s3.amazonaws.com/json-c_releases/releases/json-c-0.16.tar.gz
ARCHIVE_NAME=json-c-0.16

all: download_test extract_test config_test
	$(MAKE) -j$(HOST_NCPU) -C $(SRC_NAME)/build

download_test:
	( if [ ! -f $(ARCHIVE_NAME).tar.gz ]; then \
		wget -t5 --timeout=20 --no-check-certificate $(SRC_URL); \
	fi )

extract_test:
	( if [ ! -d $(SRC_NAME) ]; then \
		tar zxf $(ARCHIVE_NAME).tar.gz; \
		patch -p1 -d $(SRC_NAME) < json-c-0.16.patch; \
	fi )

config_test:
	( if [ -f ./config_done ]; then \
		echo "the same configuration"; \
	else \
		make configure && touch config_done; \
	fi )

configure:
	( mkdir -p $(SRC_NAME)/build && cd $(SRC_NAME)/build ; \
	cmake -DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_INSTALL_PREFIX=$(STAGEDIR) \
	-DCMAKE_TOOLCHAIN_FILE=$(CONFIG_CMAKE_TOOLCHAIN_FILE) \
	-DDISABLE_EXTRA_LIBS=TRUE \
	.. \
	)

clean:
	rm -rf $(SRC_NAME)/build
	rm -f config_done

install:
	$(MAKE) -C $(SRC_NAME)/build install DESTDIR=""

romfs:
	cp -fP $(DESTDIR)/lib/libjson-c.so* $(ROMFSDIR)/lib
