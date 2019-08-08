/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

.intel_syntax noprefix

cancel_hook:
        popad
        ret

.macro cc_before func
        pushad
        // Push pointer to original arguments.
        push esp
        add DWORD PTR ds:[esp],0x24
        call \func
        add esp, 4
        // If this function returns false, block the execution of the next.
        test al,al
        je cancel_hook
        popad
.endm

.macro cc_after func
        pushad
        // Push pointer to result.
        push esp
        add DWORD PTR ds:[esp],0x20
        call \func
        add esp, 4
        popad
.endm

.att_syntax
