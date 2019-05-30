/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#pragma once
#include "../../types.hpp"

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
