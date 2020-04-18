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
