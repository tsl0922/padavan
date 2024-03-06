.EXPORT_ALL_VARIABLES:
.PHONY: all romfs clean

ifndef ROOTDIR
ROOTDIR=..
endif
ifndef ROMFSDIR
ROMFSDIR=$(ROOTDIR)/romfs
endif

include $(PROJECT_CONFIG)
include $(ARCH_CONFIG)

DESTDIR=$(ROOTDIR)/stage

LIBC=$(if $(findstring musl,$(CROSS_COMPILE)),musl,uclibc)

all:
	$(MAKE) -C $(LIBC)

%_only:
	$(MAKE) -C $(@:_only=)

%_romfs:
	$(MAKE) -C $(@:_romfs=) romfs

%_clean:
	$(MAKE) -C $(@:_clean=) clean

romfs:
	$(MAKE) -C $(LIBC) romfs

clean:
	$(MAKE) -C $(LIBC) clean
