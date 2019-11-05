#
# Vulpes (c) 2019 gbMichelle
#
# This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
#

#include "shared.S"

.intel_syntax noprefix

.globl _game_tick_wrapper
_game_tick_wrapper:
        cc_before_pass_input _before_tick

        // No args to push because all args
        // for this function are in the registers.
        push [esp+4]
        call game_tick_actual
        add esp, 4

        cc_after_no_pass _after_tick

        ret
    game_tick_actual:
        push ecx
        push ebx
        push 0xFFFFF
        jmp [_game_tick_actual_jmp]

.att_syntax
