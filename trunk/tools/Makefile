# ===============================
#   Padavan Tools Makefile
#   Author: TurBoTse
# ===============================

CC        = gcc
CXX       = g++
CFLAGS    = -Wall -O2
LDFLAGS   =

# Optional Linux kernel configuration
-include $(LINUX_CONFIG)

# Tools to build
SUBDIRS   = lzma mkimage mksquashfs_xz sstrip

# Default target
.DEFAULT_GOAL := all

.PHONY: all clean $(SUBDIRS)

all:
	@for dir in $(SUBDIRS); do \
		echo "\033[1;30;43m BUILD \033[0m \033[1;30;44m $$dir \033[0m"; \
		$(MAKE) -C $$dir || exit $$?; \
	done

clean:
	@for dir in $(SUBDIRS); do \
		echo "\033[1;30;42m CLEAN \033[0m \033[1;30;44m $$dir \033[0m"; \
		$(MAKE) -C $$dir clean || true; \
	done
