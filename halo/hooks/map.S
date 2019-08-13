#
# Vulpes (c) 2019 gbMichelle
#
# This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
#

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
