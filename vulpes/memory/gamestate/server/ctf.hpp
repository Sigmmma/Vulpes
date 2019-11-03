/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#pragma once
#include "../../types.hpp"

struct CtfGlobals {
    Vec3d*   flag_positions[2];
    MemRef   flag_objects[2];
    int32_t  team_score[2];
    int32_t  score_limit;
    bool     flag_taken[2];
    PAD(2);
    uint32_t flag_taken_sound_timer[2];
    uint32_t assault_flag_swap_timer;
    uint32_t flag_failure_sound_timer;
    uint8_t  assault_team_with_flag;
    PAD(3);
}; static_assert(sizeof(CtfGlobals) == 0x34);
