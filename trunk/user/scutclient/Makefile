# https://github.com/scutclient/scutclient.git
SRC_NAME = scutclient-3.1.3
SRC_URL = https://github.com/scutclient/scutclient/archive/refs/tags/v3.1.3.tar.gz
THISDIR = $(shell pwd)

all: download_test extract_test config_test
	$(MAKE) -j$(HOST_NCPU) -C $(SRC_NAME)/build

download_test: 
	( if [ ! -f $(THISDIR)/$(SRC_NAME).zip ]; then \
		wget -t5 --timeout=20 --no-check-certificate -O $(SRC_NAME).zip $(SRC_URL); \
	fi )
	
extract_test:
	( if [ ! -d $(THISDIR)/$(SRC_NAME) ]; then \
		tar -xf $(SRC_NAME).tar.gz; \
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
		-DCMAKE_TOOLCHAIN_FILE=$(CONFIG_CMAKE_TOOLCHAIN_FILE) \
		.. \
	)

clean:
	rm -rf $(SRC_NAME)/build
	rm -f config_done

romfs:
	${ROMFSINST} $(THISDIR)/scripts/ /usr/bin/
	$(ROMFSINST) -p +x $(THISDIR)/$(SRC_NAME)/build/scutclient /usr/bin/bin_scutclient
