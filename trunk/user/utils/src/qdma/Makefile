EXEC = qdma
CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/drivers/net/ethernet/raeth

SRC = qdma.c
CFLAGS += -DCONFIG_RALINK_MT7621

all: $(EXEC)

qdma: $(SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(SRC) $(LDLIBS)

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o
