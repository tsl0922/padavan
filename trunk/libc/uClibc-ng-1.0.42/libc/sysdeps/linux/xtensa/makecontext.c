/* Copyright (C) 2018 - 2022 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

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
   <http://www.gnu.org/licenses/>.  */

#include <sysdep.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <ucontext.h>

extern void __start_context (void);

#if defined(__XTENSA_CALL0_ABI__)
/*
 * makecontext sets up new stack like this:
 *
 *    +------------------ stack top (uc_stack.ss_sp + uc_stack.ss_size)
 *    | optional alignment
 *    +------------------ CFA of __start_context, initial sp points here
 *    | optional padding
 *    +------------------ Optional arguments 6..argc - 1
 *    | func arg argc - 1
 *    | func arg argc - 2
 *    |  ...
 *    | func arg 6
 *    +------------------ Optional arguments 2..5
 *    | func arg 5
 * 16 | func arg 4
 *    | func arg 3
 *    | func arg 2
 *    +------------------ Optional arguments 0..1
 *    | func arg 1
 * 16 | func arg 0
 *    | padding
 *    | padding
 *    +------------------ CFA of pseudo getcontext
 *    |
 *    +------------------ stack bottom (uc_stack.ss_sp)
 *
 * When argc is 0 arguments areas are not allocated,
 * when 1 <= argc < 3 only area for arguments 0..1 is allocated,
 * when 3 <= argc < 7 areas for arguments 0..1 and 2..5 is allocated,
 * when argc >= 7 all three arguments areas are allocated.
 * Arguments 0..5 area is deallocated by the __start_context after
 * arguments are loaded into registers.
 * uc_mcontext registers are set as if __start_context made call0
 * to getcontext, sp points to that pseudo getcontext CFA.
 * setcontext/swapcontext will arrange for restoring regiters
 * a1, a12..a15 of __start_context.
 */

void
__makecontext (ucontext_t *ucp, void (*func) (void), int argc, ...)
{
  unsigned long sp = ((unsigned long) ucp->uc_stack.ss_sp
    + ucp->uc_stack.ss_size) & -16;
  int i;

  if (argc > 0)
    sp -= 4 * (argc + 2);
  sp &= -16;

  ucp->uc_mcontext.sc_pc = (unsigned long) __start_context;
  ucp->uc_mcontext.sc_a[1] = sp;
  ucp->uc_mcontext.sc_a[12] = (unsigned long) func;
  ucp->uc_mcontext.sc_a[13] = (unsigned long) ucp->uc_link;
  ucp->uc_mcontext.sc_a[14] = argc;

  if (argc)
    {
      va_list ap;

      va_start (ap, argc);
      for (i = 0; i < argc; ++i)
	((int *) sp)[i + 2] = va_arg (ap, int);
      va_end (ap);
    }
}
#elif defined(__XTENSA_WINDOWED_ABI__)
/*
 * makecontext sets up new stack like this:
 *
 *    +------------------ stack top (uc_stack.ss_sp + uc_stack.ss_size)
 *    | optional alignment
 *    +------------------ CFA of __start_context
 * 16 | Outermost caller spill area
 *    +------------------
 * 16 | __start_context overflow area
 *    +------------------ initial sp points here
 *    | optional padding
 *    +------------------ Optional arguments 6..argc - 1
 *    | func arg argc - 1
 *    | func arg argc - 2
 *    |  ...
 *    | func arg 6
 *    +------------------ Optional arguments 3..5
 *    | func arg 5
 * 16 | func arg 4
 *    | func arg 3
 *    | padding
 *    +------------------ CFA of pseudo getcontext
 * 16 | __start_context caller spill area
 *    +------------------
 *    |
 *    +------------------ stack bottom (uc_stack.ss_sp)
 *
 * When argc < 4 both arguments areas are not allocated,
 * when 4 <= argc < 7 only area for arguments 3..5 is allocated,
 * when argc >= 7 both arguments areas are allocated.
 * Arguments 3..5 area is deallocated by the __start_context after
 * arguments are loaded into registers.
 * uc_mcontext registers are set as if __start_context made call8
 * to getcontext, sp points to that pseudo getcontext CFA, spill
 * area under that sp has a1 pointing to the __start_context CFA
 * at the top of the stack. setcontext/swapcontext will arrange for
 * restoring regiters a0..a7 of __start_context.
 */

void
__makecontext (ucontext_t *ucp, void (*func) (void), int argc, ...)
{
  unsigned long sp = ((unsigned long) ucp->uc_stack.ss_sp
    + ucp->uc_stack.ss_size - 32) & -16;
  unsigned long spill0[4] =
    {
      0, sp + 32, 0, 0,
    };
  int i;

  memset ((void *) sp, 0, 32);

  if (argc > 2)
    sp -= 4 * (argc - 2);
  sp &= -16;

  ucp->uc_mcontext.sc_pc =
    ((unsigned long) __start_context & 0x3fffffff) + 0x80000000;
  ucp->uc_mcontext.sc_a[0] = 0;
  ucp->uc_mcontext.sc_a[1] = sp;
  ucp->uc_mcontext.sc_a[2] = (unsigned long) func;
  ucp->uc_mcontext.sc_a[3] = (unsigned long) ucp->uc_link;
  ucp->uc_mcontext.sc_a[4] = argc;

  if (argc)
    {
      va_list ap;

      va_start (ap, argc);
      for (i = 0; i < argc; ++i)
	{
	  if (i < 3)
	    ucp->uc_mcontext.sc_a[5 + i] = va_arg (ap, int);
	  else
	    ((int *) sp)[i - 2] = va_arg (ap, int);
	}
      va_end (ap);
    }

  sp -= 16;
  memcpy ((void *) sp, spill0, sizeof (spill0));
}
#else
#error Unsupported Xtensa ABI
#endif

weak_alias (__makecontext, makecontext)
