/*
 * This file is part of Vulpes, an extension of Halo Custom Edition's capabilities.
 * Copyright (C) 2019-2020 gbMichelle (Michelle van der Graaf)
 *
 * Vulpes is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, version 3.
 *
 * Vulpes is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * long with Vulpes.  If not, see <https://www.gnu.org/licenses/agpl-3.0.en.html>
 */

.intel_syntax noprefix

.globl _console_hook__console_in
_console_hook__console_in:
        push edi
        call _handler___process_command
        pop edi

        cmp al, 0
        jne continue_to_halo_con_in
        xor al, al
        add esp, 0x500
        ret
    continue_to_halo_con_in:
        jmp [_console_hook__return_to_halo_con_in]

.globl _console_hook__auto_complete
_console_hook__auto_complete:
        mov edx, [_console_hook__results_ptr]
        mov edx, [edx]
        # Shove all of this data into our autocomplete function
        push [_console_hook__count_ptr]
        push edx
        call _handler__auto_complete
        add esp, 8
        # move the output count to where the code expects it to be
        mov edx, [_console_hook__count_ptr]
        movsx edx, WORD PTR ds:[edx]
        ret

.att_syntax
