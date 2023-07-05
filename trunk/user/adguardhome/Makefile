VERSION := 0.107.33
VERSION_PKG := github.com/AdguardTeam/AdGuardHome/internal/version

SRC_NAME := AdGuardHome-$(VERSION)
SRC_URL := https://github.com/AdguardTeam/AdGuardHome/archive/refs/tags/v$(VERSION).tar.gz

LD_FLAGS := -s -w -buildid= \
		-X $(VERSION_PKG).channel=release \
		-X $(VERSION_PKG).version=$(VERSION)

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
		cd $(SRC_NAME); \
		patch -p1 < ../main-embed-unzip.diff; \
	fi )

build:
	( cd $(SRC_NAME); \
	NODE_OPTIONS=--openssl-legacy-provider make js-deps js-build; zip -r embed.zip build; \
	GOOS=linux GOARCH=mipsle CGO_ENABLED=0 go build -o $(THISDIR)/AdGuardHome -trimpath -ldflags "$(LD_FLAGS)"; \
	)

clean:
	rm -rf $(SRC_NAME)
	rm -f AdGuardHome

romfs:
	$(ROMFSINST) -p +x $(THISDIR)/adguardhome.sh /usr/bin/adguardhome.sh
	$(ROMFSINST) -p +x $(THISDIR)/AdGuardHome /usr/bin/AdGuardHome
