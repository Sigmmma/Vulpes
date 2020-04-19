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

void init_gamestate_upgrades();
void revert_gamestate_upgrades();

/* Another mod might have caused these to be different.
 * So, don't use these for loops and size checks.
 * Use table_instance->max_elements instead. */

static const uint16_t TABLE_OBJECT_ORIG_MAX = 2048;
static const uint16_t TABLE_OBJECT_UPGR_MAX = 4096;

static const uint16_t TABLE_CACHED_OBJECT_RENDER_STATES_ORIG_MAX = 256;
static const uint16_t TABLE_CACHED_OBJECT_RENDER_STATES_UPGR_MAX = 1024;

static const uint16_t TABLE_WIDGET_ORIG_MAX = 12;
static const uint16_t TABLE_WIDGET_UPGR_MAX = 256;

static const uint16_t TABLE_FLAG_ORIG_MAX = 2;
static const uint16_t TABLE_FLAG_UPGR_MAX = 8;

static const uint16_t TABLE_ANTENNA_ORIG_MAX = 12;
static const uint16_t TABLE_ANTENNA_UPGR_MAX = 256;

static const uint16_t TABLE_GLOW_ORIG_MAX = 8;
static const uint16_t TABLE_GLOW_UPGR_MAX = 128;

static const uint16_t TABLE_GLOW_PARTICLES_ORIG_MAX = 512;
static const uint16_t TABLE_GLOW_PARTICLES_UPGR_MAX = 2048;

static const uint16_t TABLE_LIGHT_VOLUMES_ORIG_MAX = 256;
static const uint16_t TABLE_LIGHT_VOLUMES_UPGR_MAX = 1024;

static const uint16_t TABLE_LIGHTNINGS_ORIG_MAX = 256;
static const uint16_t TABLE_LIGHTNINGS_UPGR_MAX = 1024;

static const uint16_t TABLE_DEVICE_GROUPS_ORIG_MAX = 1024;
static const uint16_t TABLE_DEVICE_GROUPS_UPGR_MAX = 2048;

static const uint16_t TABLE_LIGHTS_ORIG_MAX = 896;
static const uint16_t TABLE_LIGHTS_UPGR_MAX = 2048;

static const uint16_t TABLE_CLUSTER_LIGHT_REFERENCE_ORIG_MAX = 2048;
static const uint16_t TABLE_CLUSTER_LIGHT_REFERENCE_UPGR_MAX = 4096;

static const uint16_t TABLE_LIGHT_CLUSTER_REFERENCE_ORIG_MAX = 2048;
static const uint16_t TABLE_LIGHT_CLUSTER_REFERENCE_UPGR_MAX = 4096;

static const uint16_t TABLE_CLUSTER_COLLIDABLE_OBJECT_REFERENCE_ORIG_MAX = TABLE_OBJECT_ORIG_MAX;
static const uint16_t TABLE_CLUSTER_COLLIDABLE_OBJECT_REFERENCE_UPGR_MAX = TABLE_OBJECT_UPGR_MAX;

static const uint16_t TABLE_CLUSTER_NONCOLLIDABLE_OBJECT_REFERENCE_ORIG_MAX = TABLE_OBJECT_ORIG_MAX;
static const uint16_t TABLE_CLUSTER_NONCOLLIDABLE_OBJECT_REFERENCE_UPGR_MAX = TABLE_OBJECT_UPGR_MAX;

static const uint16_t TABLE_COLLIDABLE_OBJECT_CLUSTER_REFERENCE_ORIG_MAX = TABLE_OBJECT_ORIG_MAX;
static const uint16_t TABLE_COLLIDABLE_OBJECT_CLUSTER_REFERENCE_UPGR_MAX = TABLE_OBJECT_UPGR_MAX;

static const uint16_t TABLE_NONCOLLIDABLE_OBJECT_CLUSTER_REFERENCE_ORIG_MAX = TABLE_OBJECT_ORIG_MAX;
static const uint16_t TABLE_NONCOLLIDABLE_OBJECT_CLUSTER_REFERENCE_UPGR_MAX = TABLE_OBJECT_UPGR_MAX;

static const uint16_t TABLE_DECALS_ORIG_MAX = 2048;
static const uint16_t TABLE_DECALS_UPGR_MAX = 2048;

static const uint16_t TABLE_PLAYERS_ORIG_MAX = 16;
static const uint16_t TABLE_PLAYERS_UPGR_MAX = 32;

static const uint16_t TABLE_TEAMS_ORIG_MAX = 16;
static const uint16_t TABLE_TEAMS_UPGR_MAX = 32;

static const uint16_t TABLE_CONTRAIL_ORIG_MAX = 512;
static const uint16_t TABLE_CONTRAIL_UPGR_MAX = 2048;

static const uint16_t TABLE_CONTRAIL_POINT_ORIG_MAX = 1024;
static const uint16_t TABLE_CONTRAIL_POINT_UPGR_MAX = 4096;

// Can't upgrade this number until we've upgraded the particle code to be able
// to handle more than 1024.
static const uint16_t TABLE_PARTICLE_ORIG_MAX = 1024;
static const uint16_t TABLE_PARTICLE_UPGR_MAX = 1024;

static const uint16_t TABLE_EFFECT_ORIG_MAX = 256;
static const uint16_t TABLE_EFFECT_UPGR_MAX = 4096;

static const uint16_t TABLE_EFFECT_LOCATION_ORIG_MAX = 512;
static const uint16_t TABLE_EFFECT_LOCATION_UPGR_MAX = 8192;

static const uint16_t TABLE_PARTICLE_SYSTEMS_ORIG_MAX = 64;
static const uint16_t TABLE_PARTICLE_SYSTEMS_UPGR_MAX = 1024;

static const uint16_t TABLE_PARTICLE_SYSTEM_PARTICLES_ORIG_MAX = 512;
static const uint16_t TABLE_PARTICLE_SYSTEM_PARTICLES_UPGR_MAX = 4096;

static const uint16_t TABLE_OBJECT_LOOPING_SOUNDS_ORIG_MAX = 1024;
static const uint16_t TABLE_OBJECT_LOOPING_SOUNDS_UPGR_MAX = TABLE_OBJECT_UPGR_MAX;

static const uint16_t TABLE_ACTOR_ORIG_MAX = 256;
static const uint16_t TABLE_ACTOR_UPGR_MAX = 1024;

static const uint16_t TABLE_SWARM_ORIG_MAX = 32;
static const uint16_t TABLE_SWARM_UPGR_MAX = 32;

static const uint16_t TABLE_SWARM_COMPONENT_ORIG_MAX = 256;
static const uint16_t TABLE_SWARM_COMPONENT_UPGR_MAX = 256;

static const uint16_t TABLE_PROP_ORIG_MAX = 768;
static const uint16_t TABLE_PROP_UPGR_MAX = TABLE_OBJECT_UPGR_MAX;

static const uint16_t TABLE_ENCOUNTER_ORIG_MAX = 128;
static const uint16_t TABLE_ENCOUNTER_UPGR_MAX = 256;

static const uint16_t TABLE_AI_PERSUIT_ORIG_MAX = TABLE_ACTOR_ORIG_MAX;
static const uint16_t TABLE_AI_PERSUIT_UPGR_MAX = TABLE_ACTOR_UPGR_MAX;

static const uint16_t TABLE_OBJECT_LIST_HEADER_ORIG_MAX = 48;
static const uint16_t TABLE_OBJECT_LIST_HEADER_UPGR_MAX = 48;

static const uint16_t TABLE_LIST_OBJECT_REFERENCE_ORIG_MAX = 128;
static const uint16_t TABLE_LIST_OBJECT_REFERENCE_UPGR_MAX = 128;

static const uint16_t TABLE_HS_THREAD_ORIG_MAX = 256;
static const uint16_t TABLE_HS_THREAD_UPGR_MAX = 256;

static const uint16_t TABLE_HS_GLOBALS_ORIG_MAX = 1024;
static const uint16_t TABLE_HS_GLOBALS_UPGR_MAX = 1024;

static const uint16_t TABLE_RECORDED_ANIMATIONS_ORIG_MAX = 64;
static const uint16_t TABLE_RECORDED_ANIMATIONS_UPGR_MAX = 64;
