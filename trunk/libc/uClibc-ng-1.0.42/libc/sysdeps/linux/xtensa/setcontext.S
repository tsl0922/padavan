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
#include "ucontext_i.h"

#if defined(__XTENSA_CALL0_ABI__)
ENTRY(__setcontext)
	addi	sp, sp, -16
	s32i	a0, sp, 0
	s32i	a2, sp, 4

	addi	a3, a2, UCONTEXT_SIGMASK
	movi	a4, 0
	movi	a2, SIG_SETMASK
	movi	a5, JUMPTARGET (sigprocmask)
	callx0	a5
	bnez	a2, .Lerror

	l32i	a2, sp, 4
	l32i	a0, a2, MCONTEXT_SC_PC
	l32i	a1, a2, MCONTEXT_SC_A_0 + 4

	/* load callee-saved registers from the context */
	l32i	a12, a2, MCONTEXT_SC_A_0 + 48
	l32i	a13, a2, MCONTEXT_SC_A_0 + 52
	l32i	a14, a2, MCONTEXT_SC_A_0 + 56
	l32i	a15, a2, MCONTEXT_SC_A_0 + 60
	movi	a2, 0
	ret
.Lerror:
	l32i	a0, sp, 0
	addi	sp, sp, 16
	ret
END(__setcontext)
#elif defined(__XTENSA_WINDOWED_ABI__)
ENTRY(__setcontext)
	movi	a6, SIG_SETMASK
	addi	a7, a2, UCONTEXT_SIGMASK
	movi	a8, 0
	movi	a4, JUMPTARGET (sigprocmask)
	callx4	a4
	bnez	a6, .Lerror
	movi	a4, __window_spill
	callx4	a4

	l32i	a0, a2, MCONTEXT_SC_PC

	/* copy registers a0..a3 to spill area */
	addi	a3, a1, -16
	l32i	a4, a2, MCONTEXT_SC_A_0 + 0
	l32i	a5, a2, MCONTEXT_SC_A_0 + 4
	l32i	a6, a2, MCONTEXT_SC_A_0 + 8
	l32i	a7, a2, MCONTEXT_SC_A_0 + 12
	s32i	a4, a3, 0
	s32i	a5, a3, 4
	s32i	a6, a3, 8
	s32i	a7, a3, 12

	/* if it was call4 then register setup is done */
	extui	a4, a0, 30, 2
	bltui	a4, 2, 1f

	/* otherwise load spill overflow area address into a3 */
	addi	a3, a5, -16
	l32i	a3, a3, 4
	addi	a3, a3, -32
	beqi	a4, 2, 2f

	/* copy registers a8..a11 to spill overflow area */
	addi	a3, a3, -16
	l32i	a4, a2, MCONTEXT_SC_A_0 + 32
	l32i	a5, a2, MCONTEXT_SC_A_0 + 36
	l32i	a6, a2, MCONTEXT_SC_A_0 + 40
	l32i	a7, a2, MCONTEXT_SC_A_0 + 44
	s32i	a4, a3, 16
	s32i	a5, a3, 20
	s32i	a6, a3, 24
	s32i	a7, a3, 28

	/* copy registers a4..a7 to spill overflow area */
2:
	l32i	a4, a2, MCONTEXT_SC_A_0 + 16
	l32i	a5, a2, MCONTEXT_SC_A_0 + 20
	l32i	a6, a2, MCONTEXT_SC_A_0 + 24
	l32i	a7, a2, MCONTEXT_SC_A_0 + 28
	s32i	a4, a3, 0
	s32i	a5, a3, 4
	s32i	a6, a3, 8
	s32i	a7, a3, 12
1:
	movi	a2, 0
	retw
.Lerror:
	mov	a2, a6
	retw
END(__setcontext)
#else
#error Unsupported Xtensa ABI
#endif

weak_alias (__setcontext, setcontext)
