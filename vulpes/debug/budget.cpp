/*
 * Vulpes (c) 2020 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
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
