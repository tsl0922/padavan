THISDIR = $(shell pwd)
SRC = $(THISDIR)/skipdbv2
BUILD = $(THISDIR)/build

all: config_test
	$(MAKE) -j$(HOST_NCPU) -C $(BUILD)

config_test:
	( if [ -f ./config_done ]; then \
		echo "the same configuration"; \
	else \
		make configure && touch config_done; \
	fi )

configure:
	( mkdir -p $(BUILD); cd $(BUILD) ; \
	cmake -DCMAKE_INSTALL_PREFIX=/usr \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_EXE_LINKER_FLAGS="-L$(STAGEDIR)/lib -lev" \
	$(SRC) \
	)

clean:
	rm -rf $(BUILD)
	rm -f config_done

romfs:
	$(ROMFSINST) -p +x $(BUILD)/bin/skipd /usr/sbin/skipd
	$(ROMFSINST) -p +x $(BUILD)/bin/dbus /usr/sbin/dbus
	
