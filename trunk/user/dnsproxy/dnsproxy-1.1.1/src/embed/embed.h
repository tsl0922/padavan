/*
 * Copyright 2014, Vietor Liu <vietor.liu at gmail.com>
 * All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef EMBED_H
#define EMBED_H

#ifdef __cplusplus
#error "Just only support c language"
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

#if defined(__GNUC__)

#if defined(__LP64__)
#define __BIT64__
#endif

#define likely(x)     __builtin_expect(!!(x), 1)
#define unlikely(x)   __builtin_expect(!!(x), 0)
#define inline_always __inline__ __attribute__((always_inline))

#define container_of(ptr, type, member)					\
	({const typeof( ((type *)0)->member ) *__mptr = (ptr);		\
		(type *)( (char *)__mptr - __builtin_offsetof(type,member) );})

#elif defined(_MSC_VER)

#if defined(_M_X64)
#define __BIT64__
#endif

#define likely(x)     (x)
#define unlikely(x)   (x)
#define inline        __inline
#define inline_always __forceinline

#define container_of(ptr, type, member)					\
	(type *)((char *)ptr - (unsigned int)(&(((type *)0)->member)))

#else
#error "Unsupported compiler version"
#endif

#endif
