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

#include "shared.S"

.intel_syntax noprefix

.globl _map_load_sp_wrapper
_map_load_sp_wrapper:
        cc_before_pass_input _before_map_load_sp

        // No args to push because all args
        // for this function are in the registers.
        call map_load_sp_actual

        cc_after_pass_output _after_map_load_sp

        ret
    map_load_sp_actual:
        sub esp, 0xC
        push ebx
        push ebp
        jmp [_map_load_sp_actual_jmp]

.globl _map_load_mp_wrapper
_map_load_mp_wrapper:
        cc_before_no_pass _before_map_load_mp

        // Re-Push arguments.
        push [esp+4]
        call map_load_mp_actual
        add esp, 4

        cc_after_no_pass _after_map_load_mp

        ret
    map_load_mp_actual:
        sub esp, 0x110
        jmp [_map_load_mp_actual_jmp]

.att_syntax
