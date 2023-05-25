#https://github.com/traviscross/mtr
SRC_NAME = mtr-0.95
SRC_URL = https://github.com/traviscross/mtr/archive/refs/tags/v0.95.tar.gz
THISDIR = $(shell pwd)

all: download_test extract_test config_test
	$(MAKE) -j$(HOST_NCPU) -C $(SRC_NAME)

download_test:
	( if [ ! -f $(SRC_NAME).tar.gz ]; then \
		wget -t5 --timeout=20 --no-check-certificate -O $(SRC_NAME).tar.gz $(SRC_URL); \
	fi )

extract_test:
	( if [ ! -d $(SRC_NAME) ]; then \
		tar xf $(SRC_NAME).tar.gz; \
		cd $(SRC_NAME) ; \
		patch -p1 -i ../100-fix-ncurses-lib.diff ; \
	fi )

config_test:
	( if [ -f ./config_done ]; then \
		echo "the same configuration"; \
	else \
		make configure && touch config_done; \
	fi )

configure:
	( cd $(SRC_NAME) ; \
	./bootstrap.sh ; \
	./configure \
		--prefix=/usr \
		--without-gtk \
		--host=$(HOST_TARGET) \
		--build=$(HOST_BUILD) ; \
	)

clean:
	if [ -f $(SRC_NAME)/Makefile ] ; then \
		$(MAKE) -C $(SRC_NAME) distclean ; \
	fi ; \
	rm -f config_done

romfs:
	$(ROMFSINST) -p +x $(THISDIR)/$(SRC_NAME)/mtr /usr/bin/mtr
	$(ROMFSINST) -p +x $(THISDIR)/$(SRC_NAME)/mtr-packet /usr/bin/mtr-packet
