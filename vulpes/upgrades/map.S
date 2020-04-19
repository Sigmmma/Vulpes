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

.globl _read_map_file_header_wrapper
_read_map_file_header_wrapper:
        cmp BYTE PTR ss:_is_server, 0
        push edx
        mov edx, esi
        je not_a_server
        mov eax, ecx
    not_a_server:
        call _read_map_file_header_from_file
        pop edx
        ret

.globl _get_map_crc_wrapper_server
_get_map_crc_wrapper_server:
        shl ecx, 4
        pushad
        add eax, ecx
        call _get_map_crc
        popad
        mov eax, [eax+ecx+0xC]
        ret

.globl _get_map_crc_wrapper
_get_map_crc_wrapper:
        mov edx, _multiplayer_maps_list_ptr
        mov edx, [edx]
        shl eax, 4
        pushad
        add eax, edx
        call _get_map_crc
        test al,al
        jz leave_it_to_chimera
        popad
        mov ecx, [eax+edx+0xC]
        add esp, 4
        jmp [_jmp_skip_chimera]
    leave_it_to_chimera:
        popad
        mov ecx, edx
        ret

.att_syntax
