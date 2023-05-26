SRC_NAME := xray-1.7.5
SRC_URL := https://github.com/XTLS/Xray-core/archive/refs/tags/v1.7.5.tar.gz

THISDIR = $(shell pwd)

all: download_test extract_test build

download_test:
	( if [ ! -f $(SRC_NAME).tar.gz ]; then \
		wget -t5 --timeout=20 --no-check-certificate -O $(SRC_NAME).tar.gz $(SRC_URL); \
	fi )

extract_test:
	( if [ ! -d $(SRC_NAME) ]; then \
		mkdir -p $(SRC_NAME); \
		tar xf $(SRC_NAME).tar.gz -C $(SRC_NAME) --strip-components=1; \
	fi )

build:
	( cd $(SRC_NAME)/main; \
	GOOS=linux GOARCH=mipsle CGO_ENABLED=0 go build -o $(THISDIR)/xray -trimpath -ldflags "-s -w -buildid="; \
	)

clean:
	rm -rf $(SRC_NAME)
	rm -f xray

romfs:
	$(ROMFSINST) -p +x $(THISDIR)/xray /usr/bin/xray
