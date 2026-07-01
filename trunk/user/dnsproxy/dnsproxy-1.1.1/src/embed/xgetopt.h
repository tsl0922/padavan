/*
 * Copyright 2014, Vietor Liu <vietor.liu at gmail.com>
 * All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef XGETOPT_H
#define XGETOPT_H

#include "embed.h"

enum {
	xargument_no = 0,
	xargument_required,
	xargument_optional
};

struct xoption {
	char opt;
	const char *name;
	int has_arg;
	int *flag;
	int val;
};

int xgetopt(int argc, const char* argv[], const struct xoption* options, int* optind, const char **optarg);

#endif
