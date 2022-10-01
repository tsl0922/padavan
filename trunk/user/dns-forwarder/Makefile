SRC_NAME = dns-forwarder-1.2.1
THISDIR = $(shell pwd)

all: config_test
	$(MAKE) -j$(HOST_NCPU) -C $(SRC_NAME)/build

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
	$(ROMFSINST) -p +x $(THISDIR)/$(SRC_NAME)/build/dns-forwarder /usr/bin/dns-forwarder
	$(ROMFSINST) -p +x $(THISDIR)/dns-forwarder.sh /usr/bin/dns-forwarder.sh
