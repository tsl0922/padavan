/*
 * Copyright 1987, 1988, 1989 Massachusetts Institute of Technology
 * (Student Information Processing Board)
 *
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose is hereby granted, provided that
 * the names of M.I.T. and the M.I.T. S.I.P.B. not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  M.I.T. and the
 * M.I.T. S.I.P.B. make no representations about the suitability of
 * this software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 */

#include "config.h"
#include <stdio.h>
#include "ss_internal.h"

ss_data **_ss_table = (ss_data **)NULL;
char *_ss_pager_name = (char *)NULL;
