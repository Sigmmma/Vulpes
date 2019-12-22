/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <vulpes/memory/gamestate/object/object_unit.hpp>
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
