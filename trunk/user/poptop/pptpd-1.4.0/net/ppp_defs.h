#ifndef _NET_PPP_DEFS_H
#define _NET_PPP_DEFS_H 1

#define __need_time_t
#include <time.h>

#include <asm/types.h>
#include <linux/ppp_defs.h>

#ifndef __P
#ifdef __STDC__
#define __P(x)	x
#else
#define __P(x)	()
#endif
#endif

#endif /* net/ppp_defs.h */
