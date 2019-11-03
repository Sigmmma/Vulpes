/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#pragma once
#include "../memory/gamestate/object/object_unit.hpp"

__attribute__((regparm(3)))
void unit_speak(MemRef object, int16_t arg2, UnitSpeech* speech_data);
