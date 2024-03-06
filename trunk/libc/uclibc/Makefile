all:
	if [ ! -f $(DESTDIR)/.installed_uclibc ] ; then \
		mkdir -p $(DESTDIR) ; \
		touch $(DESTDIR)/.installed_uclibc ; \
	fi

romfs:
	$(ROMFSINST) -p +x $(CONFIG_CROSS_COMPILER_ROOT)/mipsel-linux-uclibc/sysroot/usr/bin/ldd /bin/ldd
	cp -fP $(CONFIG_CROSS_COMPILER_ROOT)/mipsel-linux-uclibc/sysroot/lib/libuClibc*.so* $(ROMFSDIR)/lib
	cp -fP $(CONFIG_CROSS_COMPILER_ROOT)/mipsel-linux-uclibc/sysroot/lib/libc.so.0 $(ROMFSDIR)/lib
	cp -fP $(CONFIG_CROSS_COMPILER_ROOT)/mipsel-linux-uclibc/sysroot/lib/ld-uClibc*.so* $(ROMFSDIR)/lib
	cp -fP $(CONFIG_CROSS_COMPILER_ROOT)/mipsel-linux-uclibc/sysroot/lib/libgcc_s.so.* $(ROMFSDIR)/lib
	cp -fP $(CONFIG_CROSS_COMPILER_ROOT)/mipsel-linux-uclibc/sysroot/lib/libthread_db*.so* $(ROMFSDIR)/lib

clean:
	rm -f $(DESTDIR)/.installed_uclibc
