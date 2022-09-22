SRC_NAME = libcgroup-0.41
SRC_URL = https://github.com/libcgroup/libcgroup/releases/download/v0.41/libcgroup-0.41.tar.gz
THISDIR = $(shell pwd)

all: download_test extract_test config_test
	$(MAKE) -j$(HOST_NCPU) -C $(SRC_NAME)

download_test:
	( if [ ! -f $(SRC_NAME).tar.gz ]; then \
		wget -t5 --timeout=20 --no-check-certificate -O $(SRC_NAME).tar.gz $(SRC_URL); \
	fi )

extract_test:
	( if [ ! -d $(SRC_NAME) ]; then \
		tar -xzf $(SRC_NAME).tar.gz; \
		patch -d $(SRC_NAME) -p1 -i ../010-cdefs.patch ; \
		patch -d $(SRC_NAME) -p1 -i ../020-tests.patch ; \
	fi )

config_test:
	( if [ -f ./config_done ]; then \
		echo "the same configuration"; \
	else \
		make configure && touch config_done; \
	fi )

configure:
	( cd $(SRC_NAME) ; \
	autoreconf -fi; \
	./configure \
		--prefix=/usr \
		--enable-tools \
		--disable-daemon \
		--disable-pam \
		--host=$(HOST_TARGET) \
		--build=$(HOST_BUILD) ; \
	)

clean:
	if [ -f $(SRC_NAME)/Makefile ] ; then \
		$(MAKE) -C $(SRC_NAME) distclean ; \
	fi ; \
	rm -f config_done

romfs:
	cp -fP $(THISDIR)/$(SRC_NAME)/src/.libs/libcgroup.so* $(ROMFSDIR)/lib
	$(ROMFSINST) -p +x $(THISDIR)/$(SRC_NAME)/src/tools/.libs/cgexec /usr/bin/cgexec
	$(ROMFSINST) -p +x $(THISDIR)/$(SRC_NAME)/src/tools/.libs/cgcreate /usr/bin/cgcreate
	$(ROMFSINST) -p +x $(THISDIR)/$(SRC_NAME)/src/tools/.libs/cgdelete /usr/bin/cgdelete
	$(ROMFSINST) -p +x $(THISDIR)/$(SRC_NAME)/src/tools/.libs/cgset /usr/bin/cgset
	$(ROMFSINST) -p +x $(THISDIR)/$(SRC_NAME)/src/tools/.libs/cgget /usr/bin/cgget
