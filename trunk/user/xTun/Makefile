SRC_NAME = xTun-0.6.5
SRC_URL = https://github.com/lparam/xTun/archive/v0.6.5.tar.gz
THISDIR = $(shell pwd)

all: download_test extract_test
	$(MAKE) -j$(HOST_NCPU) -C $(SRC_NAME) HOST=$(HOST_TARGET)

download_test:
	( if [ ! -f $(SRC_NAME).tar.gz ]; then \
		wget -t5 --timeout=20 --no-check-certificate -O $(SRC_NAME).tar.gz $(SRC_URL); \
	fi )

extract_test:
	( if [ ! -d $(SRC_NAME) ]; then \
		tar -xf $(SRC_NAME).tar.gz; \
	fi )

clean:
	( if [ -d $(SRC_NAME) ]; then \
		$(MAKE) -C $(SRC_NAME) clean
	fi )

romfs:
	$(STRIP) $(THISDIR)/$(SRC_NAME)/xTun
	$(ROMFSINST) -p +x $(THISDIR)/$(SRC_NAME)/xTun /usr/bin/xTun
	$(ROMFSINST) -p +x $(THISDIR)/xTun.sh /usr/bin/xTun.sh