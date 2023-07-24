# Makefile for systems with GNU tools

IFLAGS	 = -idirafter dummyinc

CFLAGS	+= -W -Wshadow -Wformat-security -D_FORTIFY_SOURCE=2

LDFLAGS	+= -ldisk -lshared
ifeq ($(CONFIG_FIRMWARE_INCLUDE_FTPD_SSL),y)
CFLAGS	+= -DVSF_BUILD_SSL
LDFLAGS	+= -lssl -lcrypto
endif

OBJS	= main.o utility.o prelogin.o ftpcmdio.o postlogin.o privsock.o \
	tunables.o ftpdataio.o secbuf.o ls.o asus_ext.o \
	postprivparent.o logging.o str.o netstr.o sysstr.o strlist.o \
	banner.o filestr.o parseconf.o secutil.o \
	ascii.o oneprocess.o twoprocess.o privops.o standalone.o hash.o \
	tcpwrap.o ipaddrparse.o access.o features.o readwrite.o opts.o \
	ssl.o sslslave.o ptracesandbox.o ftppolicy.o sysutil.o sysdeputil.o \
	seccompsandbox.o

.c.o:
	$(CC) -c $*.c $(CFLAGS) $(IFLAGS)

vsftpd: $(OBJS) 
	$(CC) -o vsftpd $(OBJS) $(LDFLAGS)

clean:
	rm -f *.o *.swp vsftpd
