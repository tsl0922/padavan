SRC_NAME = igmpproxy-0.4
SRC_URL = https://github.com/pali/igmpproxy/releases/download/0.4/igmpproxy-0.4.tar.gz

all: download_test extract_test config_test
	$(MAKE) -j$(HOST_NCPU) -C $(SRC_NAME)

download_test:
	( if [ ! -f $(SRC_NAME).tar.gz ]; then \
		wget -t5 --timeout=20 --no-check-certificate -O $(SRC_NAME).tar.gz $(SRC_URL); \
	fi )

extract_test:
	( if [ ! -d $(SRC_NAME) ]; then \
		tar -xf $(SRC_NAME).tar.gz; \
	fi )

config_test:
	( if [ -f ./config_done ]; then \
		echo "the same configuration"; \
	else \
		make configure && touch config_done; \
	fi )

configure:
	( cd $(SRC_NAME) ; \
	aclocal; autoconf; autoheader; automake --add-missing ; \
	./configure \
		--prefix=/ \
		--host=$(HOST_TARGET) \
		--build=$(HOST_BUILD) ; \
	)

clean:
	if [ -f $(SRC_NAME)/Makefile ] ; then \
		$(MAKE) -C $(SRC_NAME) distclean ; \
	fi ; \
	rm -f igmpproxy
	rm -f config_done

romfs:
	cp $(SRC_NAME)/src/igmpproxy .
	$(STRIP) igmpproxy
	$(ROMFSINST) /bin/igmpproxy
