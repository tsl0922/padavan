/*
 * Copyright 2014, Vietor Liu <vietor.liu at gmail.com>
 * All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "xgetopt.h"

#include <stdio.h>
#include <string.h>

#define BADCH  '?'
#define BADARG ':'

static const char *__progname(const char* name)
{
	const char *tmp = strrchr(name, '/');
#ifdef _WIN32
	if(tmp == NULL)
		tmp = strrchr(name, '\\');
#endif
	return tmp != NULL? tmp + 1: name;
}

int xgetopt(int argc, const char* argv[], const struct xoption* options, int* optind, const char **optarg)
{
	int i, val, match, name_len;
	const char *cur_argv, *embed_arg;

	if(argc < 1 || argv == NULL || options == NULL || optind == NULL || optarg == NULL)
		return -1;

	if(*optind < 1)
		*optind = 1;
	*optarg = NULL;

	if(*optind >= argc)
		return -1;

	match = -1;
	embed_arg = NULL;
	cur_argv = argv[(*optind)++];
	if(*cur_argv == '-') {
		if(*(++cur_argv) != '-') {
			if(*(cur_argv + 1) != '\0')
				embed_arg = cur_argv + 1;
			for(i = 0; options[i].opt != 0 || options[i].name; ++i) {
				if(options[i].opt == 0)
					continue;
				if(*cur_argv == options[i].opt) {
					match = i;
					break;
				}
			}
		}
		else {
			++cur_argv;
			if ((embed_arg = strchr(cur_argv, '=')) == NULL)
				name_len = strlen(cur_argv);
			else {
				name_len = embed_arg - cur_argv;
				++embed_arg;
			}
			for(i = 0; options[i].opt != 0 || options[i].name; ++i) {
				if(strncmp(cur_argv, options[i].name, name_len))
					continue;
				if(strlen(options[i].name) == name_len) {
					match = i;
					break;
				}
			}
		}
	}
	if(match == -1) {
		(void)fprintf(stderr, "%s: illegal option -- %s\n", __progname(argv[0]), cur_argv);
		return BADCH;
	}

	if(options[match].has_arg == xargument_optional
		|| options[match].has_arg == xargument_required) {
		if(embed_arg)
			*optarg = embed_arg;
		else if(*optind < argc && argv[*optind][0] != '-')
			*optarg = argv[(*optind)++];
		else if(options[match].has_arg == xargument_required) {
			(void)fprintf(stderr, "%s: option requires an argument -- %s\n", __progname(argv[0]), cur_argv);
			return BADARG;
		}
	}

	if(options[match].val == -1)
		val = (int)options[i].opt;
	else
		val = options[match].val;

	if(options[match].flag) {
		*options[match].flag = val;
		return 0;
	}
	return val;
}
