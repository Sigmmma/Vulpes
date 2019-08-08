/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include "shared.S"

.intel_syntax noprefix

.globl _weapon_pull_trigger_wrapper
_weapon_pull_trigger_wrapper:
        cc_before _before_weapon_pull_trigger

        // Re-Push arguments.
        push [esp+8]
        push [esp+8]
        call weapon_pull_trigger_actual
        add esp, 8

        cc_after _after_weapon_pull_trigger

        ret
    weapon_pull_trigger_actual:
        sub esp, 0x94
        jmp [_after_weapon_hook__pull_trigger]

.att_syntax
