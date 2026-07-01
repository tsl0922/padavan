/*
 * Copyright 2014, Vietor Liu <vietor.liu at gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or any later version. For full terms that can be
 * found in the LICENSE file.
 */

#ifdef _WIN32
#define _WIN32_WINNT 0x0501
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <io.h>
#define socklen_t int
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#define SOCKET int
#define INVALID_SOCKET -1
#define closesocket close
#endif

#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "embed/list.h"
#include "embed/rbtree.h"
#include "embed/xgetopt.h"

#pragma pack(push,1)

typedef struct {
	unsigned short id;       // identification number
	unsigned char rd :1;     // recursion desired
	unsigned char tc :1;     // truncated message
	unsigned char aa :1;     // authoritive answer
	unsigned char opcode :4; // purpose of message
	unsigned char qr :1;     // query/response flag
	unsigned char rcode :4;  // response code
	unsigned char cd :1;     // checking disabled
	unsigned char ad :1;     // authenticated data
	unsigned char z :1;      // its z! reserved
	unsigned char ra :1;     // recursion available
	unsigned short qd_count; // number of question entries
	unsigned short an_count; // number of answer entries
	unsigned short ns_count; // number of authority entries
	unsigned short nr_count; // number of resource entries
} DNS_HDR;

typedef struct {
	unsigned short type;
	unsigned short classes;
} DNS_QDS;

typedef struct {
	unsigned short type;
	unsigned short classes;
	unsigned int ttl;
	unsigned short rd_length;
	char rd_data[0];
} DNS_RRS;

#pragma pack(pop)

#define MIN_TTL 10
#define MAX_TTL (30 * 60)

typedef struct {
	struct rbnode rb_name;
	union {
		struct {
			char *prefix;
			int p_length;
			int d_same;
			struct list_head lh_name;
		};
		struct {
			time_t expire;
			struct rbnode rb_expire;
		};
	};
	time_t timestamp;
	char *domain;
	int d_length;
	char *answer;
	unsigned short an_count;
	unsigned short an_length;
	char buffer[0];
} DOMAIN_CACHE;

void domain_cache_init(const char* file);
DOMAIN_CACHE* domain_cache_search(char* domain);
void domain_cache_append(char* domain, int d_length, unsigned int ttl, unsigned short an_count, unsigned short an_length, char *answer);
void domain_cache_clean(time_t current);

typedef struct {
	struct rbnode rb_new;
	struct list_head list;
	void *context;
	time_t expire;
	unsigned short new_id;
	unsigned short old_id;
	struct sockaddr_in source;
} TRANSPORT_CACHE;

void transport_cache_init(unsigned short timeout);
TRANSPORT_CACHE* transport_cache_search(unsigned short new_id);
TRANSPORT_CACHE* transport_cache_insert(unsigned short old_id, struct sockaddr_in *address, void *context);
void transport_cache_delete(TRANSPORT_CACHE *cache);
void transport_cache_clean(time_t current);
