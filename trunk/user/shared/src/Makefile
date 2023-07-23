ifndef ROOTDIR
ROOTDIR=../..
endif

include $(BOARD_CONFIG)
include $(CFLAGS_CONFIG)

DESTDIR = /usr
INSTALL = install

CFLAGS += -fPIC -D_GNU_SOURCE
CFLAGS += -Wall -I. -I./include -I$(ROOTDIR)/$(LINUXDIR)/include/nvram

LDFLAGS += -L.

OBJS := shutils.o netutils.o rtutils.o defaults.o nvram_linux.o notify_rc.o bin_sem_asus.o flash_mtd.o pids.o gpioutils.o

HEADERS := $(shell ls *.h)
HEADERS += $(shell ls include/*.h)

all: libshared.so

libshared.so: $(OBJS)
	$(LD) -shared -o $@ $^

install:
	$(INSTALL) -d $(DESTDIR)/lib
	$(INSTALL) -m 755 libshared.so $(DESTDIR)/lib
	$(INSTALL) -d $(DESTDIR)/include/shared
	$(INSTALL) -m 644 $(HEADERS) $(DESTDIR)/include/shared

clean:
	rm -f *.o libshared.so
