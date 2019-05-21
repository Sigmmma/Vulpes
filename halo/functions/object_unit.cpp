#include "object_unit.hpp"
#include "../../hooker/hooker.hpp"
#include "../memory/gamestate/object/object_unit.hpp"

// Avoid calling these if you aren't sure about what you're doing.
// A LOT of these functions don't have any checks to see if you gave them valid input.
// Use the class functions in ObjectUnit instead.

Signature(false, sig_unit_speak_func,
    {0x53, 0x56, 0x8B, 0xF1, 0x8B, 0x0D, -1, -1, -1, -1, 0x8B, 0x49, 0x34});

__attribute__((regparm(3)))
void unit_speak(MemRef object, int16_t arg2, UnitSpeech* speech_data){
    static uintptr_t func_unit_speak_ptr = sig_unit_speak_func.address();
    if (func_unit_speak_ptr){
        asm (
            "push edi;"
            "call %0;"
            "pop edi;"
            :
            : "m" (func_unit_speak_ptr)
        );
    };
}
