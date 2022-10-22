#ifndef UL_NG_CRC32_H
#define UL_NG_CRC32_H

#include <sys/types.h>
#include <stdint.h>

extern uint32_t ul_crc32(uint32_t seed, const unsigned char *buf, size_t len);
extern uint32_t ul_crc32_exclude_offset(uint32_t seed, const unsigned char *buf, size_t len,
		                              size_t exclude_off, size_t exclude_len);

#endif

