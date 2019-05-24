/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#pragma once
#include "../event.hpp"

DEFINE_EVENT_HOOK(EVENT_PRE_TICK, void);
DEFINE_EVENT_HOOK(EVENT_TICK, void);

void init_tick_hook();
void revert_tick_hook();
