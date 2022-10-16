V2RAY_VERSION := 5.1.0
V2RAY_URL := https://codeload.github.com/v2fly/v2ray-core/tar.gz/v$(V2RAY_VERSION)
V2RAY_DIR = "v2ray-core/core/main"

THISDIR = $(shell pwd)

all: download build_extract build

download:
	( if [ ! -f $(THISDIR)/v2ray-core-$(V2RAY_VERSION).tar.gz ]; then \
	curl --create-dirs -L $(V2RAY_URL) -o $(THISDIR)/v2ray-core-$(V2RAY_VERSION).tar.gz ; \
	fi )

build_extract:
	mkdir -p $(THISDIR)/v2ray-core
	mkdir -p $(THISDIR)/bin
	( if [ ! -d $(THISDIR)/v2ray-core/core ]; then \
	tar zxf $(THISDIR)/v2ray-core-$(V2RAY_VERSION).tar.gz -C $(THISDIR)/v2ray-core ; \
	mv $(THISDIR)/v2ray-core/v2ray-core-$(V2RAY_VERSION) $(THISDIR)/v2ray-core/core ; \
	fi )

build:
	( cd $(THISDIR)/$(V2RAY_DIR); \
	GOOS=linux GOARCH=mipsle go build -gcflags=all="-l" -ldflags "-w -s -buildid=" -trimpath -o $(THISDIR)/bin/v2ray; \
	)

clean:
	rm -rf $(THISDIR)/v2ray-core
	rm -rf $(THISDIR)/bin

romfs:
	$(ROMFSINST) -p +x $(THISDIR)/bin/v2ray /usr/bin/v2ray