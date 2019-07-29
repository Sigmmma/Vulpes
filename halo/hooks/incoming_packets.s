#
# Vulpes (c) 2019 gbMichelle
#
# This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
#

.intel_syntax noprefix

.globl _incoming_packets__hook_hud_chat_intercept
_incoming_packets__hook_hud_chat_intercept:
        cmp al, 0
        je abort
        // Our own code starts here.
        push eax
        push ebx
        push ecx
        push edx
        // Call process_packet()
        lea ebx, [esp+0x10+0xC]
        push ebx
        call _process_hud_chat_message
        add esp, 4
        // If it returns false, cancel the original function by returning.
        // If it returns true, go back to the original function.
        cmp al, 0
        pop edx
        pop ecx
        pop ebx
        pop eax
        jne revert_to_original_code
        // Copy of the original way the function aborts.
        // This is so we can abort the game's processing of this packet.
    abort:
        pop edi
        pop esi
        pop ebx
        mov esp, ebp
        pop ebp
        ret
        // Restore the original registers.
    revert_to_original_code:
        jmp [_incoming_packets__jmp_hud_chat_original_code]

.att_syntax
