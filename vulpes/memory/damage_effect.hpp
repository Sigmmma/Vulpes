/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#pragma once

#include <cstdint>

enum class DamageCategory : int16_t {
    NONE = 0,
    FALLING,
    BULLET,
    GRENADE,
    HIGH_EXPLOSIVE,
    SNIPER,
    MELEE,
    FLAME,
    MOUNTED_WEAPON,
    VEHICLE,
    PLASMA,
    NEEDLE,
    SHOTGUN
};
