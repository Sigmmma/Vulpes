/*
 * Vulpes (c) 2020 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#pragma once

#include <vulpes/event.hpp>

DEFINE_EVENT_HOOK(EVENT_BEFORE_SAVE, void);
DEFINE_EVENT_HOOK(EVENT_BEFORE_LOAD, void);
DEFINE_EVENT_HOOK(EVENT_AFTER_LOAD,  void);

void init_save_load_hook();
void revert_save_load_hook();
