/* Get/set resource limits.  Linux specific syscall.
   Copyright (C) 2021-2022 Free Software Foundation, Inc.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */

#include <sys/resource.h>
#include <sysdep.h>
#include <bits/kernel-features.h>

#if defined __ASSUME_PRLIMIT64
int
prlimit (__pid_t pid, enum __rlimit_resource resource,
	     const struct rlimit *new_rlimit, struct rlimit *old_rlimit)
{
  return INLINE_SYSCALL (prlimit64, 4, pid, resource, new_rlimit,
			      old_rlimit);
}
#endif
