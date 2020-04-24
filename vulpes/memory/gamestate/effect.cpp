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

#include <vulpes/memory/signatures/signatures.hpp>

#include "effect.hpp"

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
