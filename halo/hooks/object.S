/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include "shared.S"

.intel_syntax noprefix

.globl _object_create_wrapper
_object_create_wrapper:
        cc_before_pass_input _before_object_create

        // Re-Push arguments.
        push [esp+8]
        push [esp+8]
        call object_create_actual
        add esp, 8

        cc_after_pass_output _after_object_create

        ret
    object_create_actual:
        sub esp, 0x21C
        jmp [_after_object_hook__create]


.globl _biped_jump_wrapper
_biped_jump_wrapper:
        cc_before_pass_input _before_biped_jump

        // Re-Push arguments.
        push [esp+4]
        call biped_jump_actual
        add esp, 4

        cc_after_pass_output _after_biped_jump

        ret
    biped_jump_actual:
        mov edx, [ecx+0x34]
        sub esp, 0x10
        jmp [_after_biped_hook__jump]


.globl _weapon_pull_trigger_wrapper
_weapon_pull_trigger_wrapper:
        cc_before_pass_input _before_weapon_pull_trigger

        // Re-Push arguments.
        push [esp+8]
        push [esp+8]
        call weapon_pull_trigger_actual
        add esp, 8

        cc_after_pass_output _after_weapon_pull_trigger

        ret
    weapon_pull_trigger_actual:
        sub esp, 0x94
        jmp [_after_weapon_hook__pull_trigger]

.att_syntax
