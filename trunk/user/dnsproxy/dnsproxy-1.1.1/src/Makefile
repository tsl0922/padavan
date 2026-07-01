# Makefile for dnsproxy

uname_S := $(shell sh -c 'uname -s 2>/dev/null || echo not')
version := $(shell sh -c 'cat VERSION')

CC = gcc
RM = rm -f
CP = cp -f

CFLAGS = -O2 -Wall -DVERSION=\"${version}\" -DNDEBUG
LDFLAGS = -s
PREFIX = /usr

TARGET = dnsproxy
INCLUDES = $(wildcard *.h embed/*.h)
SOURCES = $(wildcard *.c embed/*.c)
OBJS = $(patsubst %.c,%.o,$(SOURCES))

ifneq (,$(findstring MINGW,$(uname_S)))
	CFLAGS +=
	LDFLAGS += -lws2_32 -lmswsock
	TARGET := $(TARGET).exe
endif

all: $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJS): $(SOURCES) $(INCLUDES)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)

clean:
	$(RM) *.o embed/*.o *~ $(TARGET)

install:
	$(CP) $(TARGET) $(PREFIX)/sbin

uninstall:
	$(RM) $(PREFIX)/sbin/$(TARGET)
