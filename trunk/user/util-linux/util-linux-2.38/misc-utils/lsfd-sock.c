/*
 * lsfd-sock.c - handle associations opening socket objects
 *
 * Copyright (C) 2021 Red Hat, Inc. All rights reserved.
 * Written by Masatake YAMATO <yamato@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it would be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <sys/types.h>
#include <sys/xattr.h>

#include "xalloc.h"
#include "nls.h"
#include "libsmartcols.h"

#include "lsfd.h"

struct sock {
	struct file file;
	char *protoname;
};

static bool sock_fill_column(struct proc *proc __attribute__((__unused__)),
			     struct file *file,
			     struct libscols_line *ln,
			     int column_id,
			     size_t column_index)
{
	char *str = NULL;
	struct sock *sock = (struct sock *)file;
	switch(column_id) {
	case COL_TYPE:
		if (scols_line_set_data(ln, column_index, "SOCK"))
			err(EXIT_FAILURE, _("failed to add output data"));
		return true;
	case COL_PROTONAME:
		if (sock->protoname)
			if (scols_line_set_data(ln, column_index, sock->protoname))
				err(EXIT_FAILURE, _("failed to add output data"));
		return true;
	case COL_NAME:
		if (sock->protoname
		    && file->name && strncmp(file->name, "socket:", 7) == 0) {
			xasprintf(&str, "%s:%s", sock->protoname, file->name + 7);
			break;
		}
		return false;
	case COL_SOURCE:
		if (major(file->stat.st_dev) == 0
		    && strncmp(file->name, "socket:", 7) == 0) {
			str = strdup("sockfs");
			break;
		}
		return false;
	default:
		return false;
	}

	if (!str)
		err(EXIT_FAILURE, _("failed to add output data"));
	if (scols_line_refer_data(ln, column_index, str))
		err(EXIT_FAILURE, _("failed to add output data"));
	return true;
}

static void init_sock_content(struct file *file)
{
	int fd;

	assert(file);

	fd = file->association;

	if (fd >= 0 || fd == -ASSOC_MEM || fd == -ASSOC_SHM) {
		struct sock *sock = (struct sock *)file;
		char path[PATH_MAX] = {'\0'};
		char buf[256];
		ssize_t len;

		assert(file->proc);

		if (fd >= 0)
			sprintf(path, "/proc/%d/fd/%d", file->proc->pid, fd);
		else
			sprintf(path, "/proc/%d/map_files/%"PRIx64 "-%" PRIx64,
				file->proc->pid,
				file->map_start,
				file->map_end);

		len = getxattr(path, "system.sockprotoname", buf, sizeof(buf) - 1);
		if (len > 0) {
			buf[len] = '\0';
			sock->protoname = xstrdup(buf);
		}
	}
}

static void free_sock_content(struct file *file)
{
	struct sock *sock = (struct sock *)file;
	if (sock->protoname) {
		free(sock->protoname);
		sock->protoname = NULL;
	}
}

const struct file_class sock_class = {
	.super = &file_class,
	.size = sizeof(struct sock),
	.fill_column = sock_fill_column,
	.initialize_content = init_sock_content,
	.free_content = free_sock_content,
};
