EXEC = mtd_write

OBJS = mtd.o crc32.o md5.o jffs2.o

CFLAGS += -D__BYTE_ORDER=__LITTLE_ENDIAN

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

clean:
	rm -f $(EXEC) *.o
