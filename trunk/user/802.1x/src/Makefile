# If you want to support multiple radius server for multiple bssid, add following line
#CFLAGS += -DMULTIPLE_RADIUS

# If you want to debug daemon, add following line
#CFLAGS += -DDBG

OBJS = rtdot1x.o eloop.o eapol_sm.o radius.o md5.o config.o \
       ieee802_1x.o sta_info.o radius_client.o

all: rt2860apd

rt2860apd: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o rt2860apd $(OBJS)

clean:
	rm -f core *~ *.o rt2860apd *.d

-include $(OBJS:%.o=%.d)
