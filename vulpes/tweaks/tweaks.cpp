/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <vulpes/hooks/tick.hpp>
#include <vulpes/memory/gamestate/console.hpp>

#include "tweaks.hpp"

void init_tweaks() {
    console_input_globals()->enabled = true;
    DEL_CALLBACK(EVENT_TICK, init_tweaks);
}

void revert_tweaks() {
    DEL_CALLBACK(EVENT_TICK, init_tweaks);
}
