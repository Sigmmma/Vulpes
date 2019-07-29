#
# Vulpes (c) 2019 gbMichelle
#
# This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
#

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
