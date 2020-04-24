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

#pragma pack(push, 1)

struct KingGlobals {
    uint32_t unknown[16];
    uint32_t unknown2[16];
    uint8_t unknown3[16];
    uint32_t netgame_flag_related;
    float hill_finding_and_score_related;
    uint32_t hill_find_related1;
    uint32_t hill_find_related2[34];
    uint32_t hill_find_related3[24];
    Vec3d find_hill_related4;
    uint32_t calculate_hill_state_related1;
    uint32_t calculate_hill_state_related2;
    uint32_t calculate_hill_state_related3;
    float hill_find_related5;
    float hill_find_related6;
    uint32_t current_hill;
    uint32_t current_hill_time_left;
    uint16_t max_hill;
    PAD(2);
    uint16_t netgame_flag_related2[64]; // Starting flag?
}; static_assert(sizeof(KingGlobals) == 0x230);

#pragma pack(pop)

struct KingGlobalsUpgrade {
    uint32_t hill_length;
};

KingGlobals* king_globals();
KingGlobalsUpgrade* king_globals_upgrade();
