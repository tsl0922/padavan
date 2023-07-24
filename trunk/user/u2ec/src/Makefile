#
# U2EC Makefile
#
# Copyright (C) 2008 ASUSTeK Corporation
#
#

PROGS	= u2ec
OBJS	= usbsock.o testusb.o
DEBUG	= n

ifeq ($(DEBUG),y)
	DEBFLAGS = -O -g -DU2EC_DEBUG
	DEBFLAGS += -DPDEBUG_SENDSECV
	DEBFLAGS += -DPDEBUG_DECODE
	OBJS += debug.o decode.o
else
	DEBFLAGS = -s
endif

CFLAGS += -DSUPPORT_LPRng
CFLAGS += -Wall -I.
CFLAGS += $(DEBFLAGS)

LDFLAGS += -lpthread
LDFLAGS += -lshared
LDFLAGS += -lusb

all: $(PROGS)

$(PROGS): $(OBJS)
	$(CC) -o $(PROGS) $(OBJS) $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(PROGS) *.o *~
