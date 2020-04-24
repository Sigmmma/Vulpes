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

enum class AiCommunicationType : int16_t {
    DEATH = 0,
    SPREE,
    HURT,
    DAMAGE,
    SIGHTED_ENEMY,
    FOUND_ENEMY,
    UNEXPECTED_ENEMY,
    FOUND_DEAD_FRIEND,
    ALLEGIANCE_CHANGED,
    GRENADE_THROWING,
    GRENADE_STARTLE,
    GRENADE_SIGHTED,
    GRENADE_DANGER,
    LOST_CONTACT,
    BLOCKED,
    ALERT_NONCOMBAT,
    SEARCH_START,
    SEARCH_QUERY,
    SEARCH_REPORT,
    SEARCH_ABANDON,
    SEARCH_GROUP_ABANDON,
    UNCOVER_START,
    ADVANCE,
    RETREAT,
    COVER,
    SIGHTED_FRIEND_PLAYER,
    SHOOTING,
    SHOOTING_VEHICLE,
    SHOOTING_BERSERK,
    SHOOTING_GROUP,
    SHOOTING_TRAITOR,
    FLEE,
    FLEE_LEADER_DIED,
    FLEE_IDLE,
    ATTEMPTED_FLEE,
    HIDING_FINISHED,
    VEHICLE_ENTRY,
    VEHICLE_EXIT,
    VEHICLE_WOOHOO,
    VEHICLE_SCARED,
    VEHICLE_FALLING,
    SURPRISE,
    BERSERK,
    MELEE,
    DIVE,
    UNCONVER_EXCLAMATION,
    FALLING,
    LEAP,
    POSTCOMBAT_ALONE,
    POSTCOMBAT_UNSCATHED,
    POSTCOMBAT_WOUNDED,
    POSTCOMBAT_MASSACRE,
    POSTCOMBAT_TRIUMPH,
    POSTCOMBAT_CHECK_ENEMY,
    POSTCOMBAT_CHECK_FRIEND,
    POSTCOMBAT_SHOOT_CORPSE,
    POSTCOMBAT_CELEBRATE
};

struct AiCommunicationPacket {
    PAD(6); //unknown
    AiCommunicationType type;
    PAD(2); //unknown
    PAD(2);
    PAD(2); //unknown
    PAD(6);
    PAD(2); //unknown
    PAD(2);
    PAD(4); //unknown
    bool broken;
    PAD(3);
}; static_assert(sizeof(AiCommunicationPacket) == 0x20);
