/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <vulpes/memory/signatures.hpp>

#include "effect.hpp"
#include "table.hpp"

static GenericTable** effect_ptrs_lst;

static inline GenericTable** effect_ptrs() {
    if (!effect_ptrs_lst) {
        effect_ptrs_lst = *reinterpret_cast<GenericTable***>(
            sig_effect_table_refs());
    }
    return effect_ptrs_lst;
}

GenericTable* weather_particles_table() {
    return effect_ptrs()[0];
}

GenericTable* particle_table() {
    return effect_ptrs()[1];
}

GenericTable* particle_system_particles_table() {
    return effect_ptrs()[2];
}

GenericTable* effect_table() {
    return effect_ptrs()[3];
}

GenericTable* effect_locations_table() {
    return effect_ptrs()[4];
}

GenericTable* decals_table() {
    return effect_ptrs()[5];
}

GenericTable* contrail_points_table() {
    return effect_ptrs()[6];
}

GenericTable* contrail_table() {
    return effect_ptrs()[7];
}
