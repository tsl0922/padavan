EXEC = ralinkiappd
OBJS = rtmpiapp.o mt_iapp_util.o

# If you want to debug daemon, add following line
CFLAGS += -I./ -DDBG -DSYSLOG -fvisibility=hidden
#-DSYSLOG
$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

clean:
	rm -f core *.o ralinkiappd
