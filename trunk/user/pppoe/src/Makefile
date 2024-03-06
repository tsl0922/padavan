EXEC = pppoe-discovery
RP_VERSION=3.12
CFLAGS += '-DRP_VERSION="$(RP_VERSION)"'

all: $(EXEC)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ -fPIC $^

pppoe-discovery:  pppoe-discovery.o discovery.o if.o common.o debug.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

clean:
	rm -f $(EXEC) *.o

