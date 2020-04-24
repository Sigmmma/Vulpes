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

#pragma once

#include <vulpes/memory/types.hpp>

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
