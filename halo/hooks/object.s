#
# Vulpes (c) 2019 gbMichelle
#
# This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
#
.intel_syntax noprefix

cancel_hook:
        popad
        ret

.globl _weapon_pull_trigger_actual
_weapon_pull_trigger_actual:
        sub esp, 0x94
        jmp [_after_weapon_hook__pull_trigger]

.globl _weapon_pull_trigger_wrapper
_weapon_pull_trigger_wrapper:
        pushad
        // Push pointer to original arguments.
        push esp
        add DWORD PTR ds:[esp],0x24
        call _before_weapon_pull_trigger
        add esp, 4
        // If this function returns false, block the execution of the next.
        test al,al
        je cancel_hook
        popad

        // Re-Push arguments.
        mov eax, [esp+8]
        push eax
        mov eax, [esp+8]
        push eax
        call _weapon_pull_trigger_actual
        add esp, 8

        pushad
        // Push pointer to original arguments.
        push esp
        add DWORD PTR ds:[esp],0x24
        call _after_weapon_pull_trigger
        add esp, 4
        popad

        ret

.att_syntax
