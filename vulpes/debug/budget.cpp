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

#include <vulpes/functions/messaging.hpp>
#include <vulpes/memory/gamestate/object/object.hpp>
#include <vulpes/memory/gamestate/effect.hpp>
#include <vulpes/network/network_id.hpp>

#define BUDGET_TEMPLATE "%s:|t% 4d/%d"

void cprint_budget() {
    console_clear();
    auto objects = object_table();
    auto wp = weather_particles_table();
    auto p = particle_table();
    auto psp = particle_system_particles_table();
    auto e = effect_table();
    auto el = effect_locations_table();
    auto d = decals_table();
    auto cp = contrail_points_table();
    auto c = contrail_table();
    auto synced_objs = synced_objects();

    // Print two collumns separated by \t

    cprintf(BUDGET_TEMPLATE "|t" BUDGET_TEMPLATE,
        objects->name, objects->count(), objects->max_elements,
        wp->name, wp->count(), wp->max_elements);


    cprintf(BUDGET_TEMPLATE "|t" BUDGET_TEMPLATE,
        p->name, p->count(), p->max_elements,
        psp->name, psp->count(), psp->max_elements
    );

    cprintf(BUDGET_TEMPLATE "|t" BUDGET_TEMPLATE,
        e->name, e->count(), e->max_elements,
        el->name, el->count(), el->max_elements
    );

    cprintf(BUDGET_TEMPLATE "|t" BUDGET_TEMPLATE,
        d->name, d->count(), d->max_elements,
        cp->name, cp->count(), cp->max_elements
    );

    cprintf(BUDGET_TEMPLATE "|t" BUDGET_TEMPLATE,
        c->name, c->count(), c->max_elements,
        "Vanilla network objects", synced_objs->count, synced_objs->max_count
    );
}
