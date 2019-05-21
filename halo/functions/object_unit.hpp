#pragma once
#include "../memory/gamestate/object/object_unit.hpp"

__attribute__((regparm(3)))
void unit_speak(MemRef object, int16_t arg2, UnitSpeech* speech_data);
