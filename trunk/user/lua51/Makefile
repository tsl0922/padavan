SRC_NAME = lua-5.1.5
THISDIR = $(shell pwd)
TARGET_CFLAGS += -DLUA_USE_LINUX -fPIC -std=gnu99

all:
	$(MAKE) -j$(HOST_NCPU) -C $(SRC_NAME) \
		CC="$(CC)" \
		AR="$(AR) rcu" \
		RANLIB="$(RANLIB)" \
		INSTALL_ROOT=/usr \
		CFLAGS="$(TARGET_CPPFLAGS) $(TARGET_CFLAGS)" \
		PKG_VERSION="5.1.5" \
		MYLDFLAGS="$(TARGET_LDFLAGS)" \
		linux
	$(MAKE) -C $(SRC_NAME) INSTALL_TOP=$(STAGEDIR) install
	$(MAKE) -C $(THISDIR)/lua-cjson-2.1.0 PREFIX=$(STAGEDIR)
	$(MAKE) -C $(THISDIR)/lua-nixio-0.3/src

clean:
	( if [ -f $(SRC_NAME)/Makefile ] ; then \
		$(MAKE) -C $(SRC_NAME) clean ; \
	fi )
	$(MAKE) -C $(THISDIR)/lua-cjson-2.1.0 clean
	$(MAKE) -C $(THISDIR)/lua-nixio-0.3/src clean

romfs:
	cp -fP $(THISDIR)/lua-5.1.5/src/liblua.so.5.1.5 $(ROMFSDIR)/lib
	cp -fP $(THISDIR)/lua-5.1.5/src/liblua.so $(ROMFSDIR)/lib
	$(ROMFSINST) $(THISDIR)/lua-5.1.5/src/lua /usr/bin/lua
	mkdir -p $(ROMFSDIR)/etc_ro/lua/5.1/lib $(ROMFSDIR)/etc_ro/lua/5.1/share
	$(ROMFSINST) $(THISDIR)/lua-cjson-2.1.0/cjson.so /etc_ro/lua/5.1/lib/cjson.so
	$(ROMFSINST) $(THISDIR)/lua-nixio-0.3/src/nixio.so /etc_ro/lua/5.1/lib/nixio.so
	$(ROMFSINST) $(THISDIR)/lua-cjson-2.1.0/lua/ /etc_ro/lua/5.1/share/
	$(ROMFSINST) $(THISDIR)/lua-nixio-0.3/root/usr/lib/lua/ /etc_ro/lua/5.1/share/