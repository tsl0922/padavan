EXEC = networkmap
OBJS = function.o networkmap.o

LDFLAGS += -lshared

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) $(CFLAGS)

clean: 
	rm -rf *.o $(EXEC) *~

