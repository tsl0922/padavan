OS_LAYER = osl-linux.c
LDFLAGS += -lshared

all: lld2d

include common.mk

depend:
	$(CC) $(CFLAGS) -M $(DCFILES) >.depend

clear:
	rm -f *.o lld2d

-include .depend
