/* Support for relocating static PIE.
   Copyright (C) 2017-2022 Free Software Foundation, Inc.
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
   <https://www.gnu.org/licenses/>.  */

#include <link.h>
#include <elf.h>
#include <dl-elf.h>

ElfW(Addr) _dl_load_base = NULL;

void
reloc_static_pie (ElfW(Addr) load_addr);

void
reloc_static_pie (ElfW(Addr) load_addr)
{
    ElfW(Word) relative_count = 0;
    ElfW(Addr) rel_addr = 0;
    ElfW(Dyn) * dyn_addr = NULL;
    unsigned long dynamic_info[DYNAMIC_SIZE] = {0};

    /* Read our own dynamic section and fill in the info array.  */
    dyn_addr = ((void *) load_addr + elf_machine_dynamic ());

    /* Use the underlying function to avoid TLS access before initialization */
    __dl_parse_dynamic_info(dyn_addr, dynamic_info, NULL, load_addr);

    /* Perform relocations */
    relative_count = dynamic_info[DT_RELCONT_IDX];
    rel_addr = dynamic_info[DT_RELOC_TABLE_ADDR];
    elf_machine_relative(load_addr, rel_addr, relative_count);
    _dl_load_base = load_addr;
}
