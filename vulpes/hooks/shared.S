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
        // eax contains the return value. 7*4 = 0x1C so look there for eax.
        push esp
        add DWORD PTR ds:[esp],0x1C
        call \func
        add esp, 4
        popad
.endm

// Protects the stack and executes the before function.
//
// Cancels the actual function and the after function if that returns false.

.macro cc_before_no_pass func
        pushad
        call \func
        // If this function returns false, block the execution of the next.
        test al,al
        je cancel_hook
        popad
.endm

// Protects the stack and executes the after function.

.macro cc_after_no_pass func
        pushad
        call \func
        popad
.endm

.att_syntax
