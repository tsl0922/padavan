SRC_NAME=msd_lite
THISDIR = $(shell pwd)

CFLAGS += -fno-unwind-tables -fno-asynchronous-unwind-tables

all: download_test config_test
	$(MAKE) -j$(HOST_NCPU) -C $(SRC_NAME)/build

download_test:
	( if [ ! -d $(THISDIR)/$(SRC_NAME) ]; then \
		git clone https://github.com/rozhuk-im/msd_lite.git ; \
		cd $(SRC_NAME) ; \
		git submodule update --init --recursive ; \
		patch -p1 -i ../msd_lite.diff ; \
	fi )

config_test:
	( if [ -f ./config_done ]; then \
		echo "the same configuration"; \
	else \
		make configure && touch config_done; \
	fi )

configure:
	( mkdir -p $(SRC_NAME)/build && cd $(SRC_NAME)/build ; \
	cmake -DCMAKE_TOOLCHAIN_FILE=../../cross-mipsel-linux.cmake -DCMAKE_BUILD_TYPE=RELEASE .. \
	)

clean:
	rm -rf $(SRC_NAME)/build
	rm -f config_done

romfs:
	$(ROMFSINST) $(THISDIR)/$(SRC_NAME)/build/src/msd_lite /usr/bin/msd_lite
	$(ROMFSINST) $(THISDIR)/msd_lite.conf /etc_ro/msd_lite.conf
