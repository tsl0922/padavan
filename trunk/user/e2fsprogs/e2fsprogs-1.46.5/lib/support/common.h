/*
 *
 *	Various things common for all utilities
 *
 */

#ifndef __QUOTA_COMMON_H__
#define __QUOTA_COMMON_H__

#if EXT2_FLAT_INCLUDES
#include "e2_types.h"
#else
#include <ext2fs/ext2_types.h>
#endif /* EXT2_FLAT_INCLUDES */

/* #define DEBUG_QUOTA 1 */

#ifndef __attribute__
# if !defined __GNUC__ || __GNUC__ < 2 || (__GNUC__ == 2 && __GNUC_MINOR__ < 8) || __STRICT_ANSI__
#  define __attribute__(x)
# endif
#endif

#define log_err(format, arg ...)					\
	fprintf(stderr, "[ERROR] %s:%d:%s: " format "\n",		\
		__FILE__, __LINE__, __func__, ## arg)

#ifdef DEBUG_QUOTA
# define log_debug(format, arg ...)					\
	fprintf(stderr, "[DEBUG] %s:%d:%s: " format "\n",		\
		__FILE__, __LINE__, __func__, ## arg)
#else
# define log_debug(...)
#endif

#endif /* __QUOTA_COMMON_H__ */
