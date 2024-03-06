DESTDIR = /usr
INSTALL = install

include $(BOARD_CONFIG)
include $(CFLAGS_CONFIG)

CFLAGS += -fPIC
LDFLAGS += -lshared

OBJS = dev_info.o disk_io_tools.o disk_initial.o disk_share.o
ifdef CONFIG_USB_SUPPORT
OBJS += usb_info.o
endif

HEADERS := $(shell ls *.h)

all: libdisk.so test_share

libdisk.so: $(OBJS)
	$(LD) -shared -o $@ $^

test_share: test_share.o libdisk.so
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

install:
	$(INSTALL) -d $(DESTDIR)/lib
	$(INSTALL) -m 755 libdisk.so $(DESTDIR)/lib
	$(INSTALL) -d $(DESTDIR)/include/libdisk
	$(INSTALL) -m 644 $(HEADERS) $(DESTDIR)/include/libdisk

clean:
	rm -f *.o libdisk.so test_share
