all:
	if [ ! -f $(DESTDIR)/.installed_musl ] ; then \
		mkdir -p $(DESTDIR) ; \
		touch $(DESTDIR)/.installed_musl ; \
	fi

romfs:
	cp -fP $(CONFIG_CROSS_COMPILER_ROOT)/mipsel-linux-musl/sysroot/usr/lib/libc.so $(ROMFSDIR)/lib
	cp -fP $(CONFIG_CROSS_COMPILER_ROOT)/mipsel-linux-musl/sysroot/lib/libgcc_s.so.* $(ROMFSDIR)/lib
	ln -sf libc.so $(ROMFSDIR)/lib/ld-musl-mipsel.so.1
	ln -sf ../lib/libc.so $(ROMFSDIR)/bin/ldd

clean:
	rm -f $(DESTDIR)/.installed_musl
