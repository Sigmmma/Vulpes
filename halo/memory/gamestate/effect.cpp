/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include "effect.hpp"
#include "table.hpp"

Table* weather_particles_table(){
    static Table** effect_ptrs_loc = effect_ptrs();
    return effect_ptrs_loc[0];
}

Table* particle_table(){
    static Table** effect_ptrs_loc = effect_ptrs();
    return effect_ptrs_loc[1];
}

Table* particle_system_particles_table(){
    static Table** effect_ptrs_loc = effect_ptrs();
    return effect_ptrs_loc[2];
}

Table* effect_table(){
    static Table** effect_ptrs_loc = effect_ptrs();
    return effect_ptrs_loc[3];
}

Table* effect_locations_table(){
    static Table** effect_ptrs_loc = effect_ptrs();
    return effect_ptrs_loc[4];
}

Table* decals_table(){
    static Table** effect_ptrs_loc = effect_ptrs();
    return effect_ptrs_loc[5];
}

Table* contrail_points_table(){
    static Table** effect_ptrs_loc = effect_ptrs();
    return effect_ptrs_loc[6];
}

Table* contrail_table(){
    static Table** effect_ptrs_loc = effect_ptrs();
    return effect_ptrs_loc[7];
}
