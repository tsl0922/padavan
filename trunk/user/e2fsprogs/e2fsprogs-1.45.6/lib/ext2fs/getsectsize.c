/*
 * getsectsize.c --- get the sector size of a device.
 *
 * Copyright (C) 1995, 1995 Theodore Ts'o.
 * Copyright (C) 2003 VMware, Inc.
 *
 * %Begin-Header%
 * This file may be redistributed under the terms of the GNU Library
 * General Public License, version 2.
 * %End-Header%
 */

#ifndef _LARGEFILE_SOURCE
#define _LARGEFILE_SOURCE
#endif
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif

#include "config.h"
#include <stdio.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_ERRNO_H
#include <errno.h>
#endif
#include <fcntl.h>
#ifdef HAVE_SYS_DISK_H
#include <sys/disk.h>
#endif
#ifdef HAVE_LINUX_FD_H
#include <sys/ioctl.h>
#include <linux/fd.h>
#endif

#if defined(__linux__) && defined(_IO)
#if !defined(BLKSSZGET)
#define BLKSSZGET  _IO(0x12,104)/* get block device sector size */
#endif
#if !defined(BLKPBSZGET)
#define BLKPBSZGET _IO(0x12,123)/* get block physical sector size */
#endif
#endif

#include "ext2_fs.h"
#include "ext2fs.h"

/*
 * Returns the logical sector size of a device
 */
errcode_t ext2fs_get_device_sectsize(const char *file, int *sectsize)
{
	int	fd;

	fd = ext2fs_open_file(file, O_RDONLY, 0);
	if (fd < 0)
		return errno;

#ifdef BLKSSZGET
	if (ioctl(fd, BLKSSZGET, sectsize) >= 0) {
		close(fd);
		return 0;
	}
#endif
#ifdef DIOCGSECTORSIZE
	if (ioctl(fd, DIOCGSECTORSIZE, sectsize) >= 0) {
		close(fd);
		return 0;
	}
#endif
	*sectsize = 0;
	close(fd);
	return 0;
}

/*
 * Return desired alignment for direct I/O
 */
int ext2fs_get_dio_alignment(int fd)
{
	int align = 0;

#ifdef BLKSSZGET
	if (ioctl(fd, BLKSSZGET, &align) < 0)
		align = 0;
#endif
#ifdef DIOCGSECTORSIZE
	if (align <= 0 &&
	    ioctl(fd, DIOCGSECTORSIZE, &align) < 0)
		align = 0;
#endif

#ifdef _SC_PAGESIZE
	if (align <= 0)
		align = sysconf(_SC_PAGESIZE);
#endif
#ifdef HAVE_GETPAGESIZE
	if (align <= 0)
		align = getpagesize();
#endif
	if (align <= 0)
		align = 4096;

	return align;
}

/*
 * Returns the physical sector size of a device
 */
errcode_t ext2fs_get_device_phys_sectsize(const char *file, int *sectsize)
{
	int	fd;

	fd = ext2fs_open_file(file, O_RDONLY, 0);
	if (fd < 0)
		return errno;

#ifdef BLKPBSZGET
	if (ioctl(fd, BLKPBSZGET, sectsize) >= 0) {
		close(fd);
		return 0;
	}
#endif
#ifdef DIOCGSECTORSIZE
	/* This isn't really the physical sector size, but FreeBSD
	 * doesn't seem to have this concept. */
	if (ioctl(fd, DIOCGSECTORSIZE, sectsize) >= 0) {
		close(fd);
		return 0;
	}
#endif
	*sectsize = 0;
	close(fd);
	return 0;
}
