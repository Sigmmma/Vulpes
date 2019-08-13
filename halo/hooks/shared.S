/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

.intel_syntax noprefix

cancel_hook:
        popad
        ret

// Protects the stack and executes the before function.
//
// It takes a pointer to the arguments on the stack. Repushing them sucks.
// But giving the function a pointer allows you to modify the variables before
// they go to the actual function.
//
// Cancels the actual function and the after function if that returns false.

.macro cc_before_pass_input func
        pushad
        // Push pointer to original arguments.
        // pushad pushes (all registers 8*4 = 0x20) + (return pointer = 4) = 0x24
        push esp
        add DWORD PTR ds:[esp],0x24
        call \func
        add esp, 4
        // If this function returns false, block the execution of the next.
        test al,al
        je cancel_hook
        popad
.endm

// Protects the stack and executes the after function.
//
// Passes the function a pointer to the return value so it can be manipulated.

.macro cc_after_pass_output func
        pushad
        // Push pointer to result.
        // pushad pushes all registers starting with eax.
        // eax contains the return value. 8*4 = 0x20 so look there for eax.
        push esp
        add DWORD PTR ds:[esp],0x20
        call \func
        add esp, 4
        popad
.endm

.att_syntax
