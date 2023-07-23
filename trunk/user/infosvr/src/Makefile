USERDIR = $(ROOTDIR)/user

CFLAGS += -I.
LDFLAGS += -lshared

OBJS=infosvr.o asus.o

all: infosvr

%.o: %.c
	$(CC) $(CFLAGS) -c $<

infosvr: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o infosvr $(LIBS)
	$(STRIP) $@

clean:
	rm -f *.o *~ infosvr

