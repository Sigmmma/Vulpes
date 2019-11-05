/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include "effect.hpp"
#include "table.hpp"

GenericTable* weather_particles_table() {
    static GenericTable** effect_ptrs_loc = effect_ptrs();
    return effect_ptrs_loc[0];
}

GenericTable* particle_table() {
    static GenericTable** effect_ptrs_loc = effect_ptrs();
    return effect_ptrs_loc[1];
}

GenericTable* particle_system_particles_table() {
    static GenericTable** effect_ptrs_loc = effect_ptrs();
    return effect_ptrs_loc[2];
}

GenericTable* effect_table() {
    static GenericTable** effect_ptrs_loc = effect_ptrs();
    return effect_ptrs_loc[3];
}

GenericTable* effect_locations_table() {
    static GenericTable** effect_ptrs_loc = effect_ptrs();
    return effect_ptrs_loc[4];
}

GenericTable* decals_table() {
    static GenericTable** effect_ptrs_loc = effect_ptrs();
    return effect_ptrs_loc[5];
}

GenericTable* contrail_points_table() {
    static GenericTable** effect_ptrs_loc = effect_ptrs();
    return effect_ptrs_loc[6];
}

GenericTable* contrail_table() {
    static GenericTable** effect_ptrs_loc = effect_ptrs();
    return effect_ptrs_loc[7];
}
