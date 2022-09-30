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
ENTRY(__swapcontext)
	s32i	a0, a2, MCONTEXT_SC_PC
	s32i	a1, a2, MCONTEXT_SC_A_0 + 4

	/* save callee-saved registers in the context */
	s32i	a12, a2, MCONTEXT_SC_A_0 + 48
	s32i	a13, a2, MCONTEXT_SC_A_0 + 52
	s32i	a14, a2, MCONTEXT_SC_A_0 + 56
	s32i	a15, a2, MCONTEXT_SC_A_0 + 60

	mov	a12, a3
	mov	a13, a2

	addi	a3, a3, UCONTEXT_SIGMASK
	addi	a4, a2, UCONTEXT_SIGMASK
	movi	a2, SIG_SETMASK
	movi	a5, JUMPTARGET (sigprocmask)
	callx0	a5
	bnez	a2, .Lerror

	mov	a2, a12
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
	l32i	a0, a13, MCONTEXT_SC_PC
	l32i	a12, a13, MCONTEXT_SC_A_0 + 48
	l32i	a13, a13, MCONTEXT_SC_A_0 + 52
	ret
END(__swapcontext)
#elif defined(__XTENSA_WINDOWED_ABI__)
ENTRY(__swapcontext)
	movi	a4, __window_spill
	callx4	a4
	mov	a9, a3
	s32i	a0, a2, MCONTEXT_SC_PC

	/* copy registers a0..a3 from spill area */
	addi	a3, a1, -16
	l32i	a4, a3, 0
	l32i	a5, a3, 4
	l32i	a6, a3, 8
	l32i	a7, a3, 12
	s32i	a4, a2, MCONTEXT_SC_A_0 + 0
	s32i	a5, a2, MCONTEXT_SC_A_0 + 4
	s32i	a6, a2, MCONTEXT_SC_A_0 + 8
	s32i	a7, a2, MCONTEXT_SC_A_0 + 12

	/* if it was call4 then register saving is done */
	extui	a4, a0, 30, 2
	bltui	a4, 2, 1f

	/* otherwise load spill overflow area address into a3 */
	addi	a3, a5, -16
	l32i	a3, a3, 4
	addi	a3, a3, -32
	beqi	a4, 2, 2f

	/* copy registers a8..a11 from spill overflow area */
	addi	a3, a3, -16
	l32i	a4, a3, 16
	l32i	a5, a3, 20
	l32i	a6, a3, 24
	l32i	a7, a3, 28
	s32i	a4, a2, MCONTEXT_SC_A_0 + 32
	s32i	a5, a2, MCONTEXT_SC_A_0 + 36
	s32i	a6, a2, MCONTEXT_SC_A_0 + 40
	s32i	a7, a2, MCONTEXT_SC_A_0 + 44

	/* copy registers a4..a7 from spill overflow area */
2:
	l32i	a4, a3, 0
	l32i	a5, a3, 4
	l32i	a6, a3, 8
	l32i	a7, a3, 12
	s32i	a4, a2, MCONTEXT_SC_A_0 + 16
	s32i	a5, a2, MCONTEXT_SC_A_0 + 20
	s32i	a6, a2, MCONTEXT_SC_A_0 + 24
	s32i	a7, a2, MCONTEXT_SC_A_0 + 28
1:
	movi	a6, SIG_SETMASK
	addi	a7, a9, UCONTEXT_SIGMASK
	addi	a8, a2, UCONTEXT_SIGMASK
	mov	a2, a9
	movi	a4, JUMPTARGET (sigprocmask)
	callx4	a4
	bnez	a6, .Lerror

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
END(__swapcontext)
#else
#error Unsupported Xtensa ABI
#endif

weak_alias (__swapcontext, swapcontext)
