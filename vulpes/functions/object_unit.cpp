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

#include <vulpes/memory/signatures.hpp>

#include "object_unit.hpp"

// Avoid calling these if you aren't sure about what you're doing.
// A LOT of these functions don't have any checks to see if you gave them valid input.
// Use the class functions in ObjectUnit instead.

__attribute__((regparm(3)))
void unit_speak(MemRef object, int16_t arg2, UnitSpeech* speech_data) {
    uintptr_t func_unit_speak_ptr = sig_func_unit_speak();
    if (func_unit_speak_ptr) {
        asm (
            "push edi;"
            "call %0;"
            "pop edi;"
            :
            : "m" (func_unit_speak_ptr)
        );
    }
}
