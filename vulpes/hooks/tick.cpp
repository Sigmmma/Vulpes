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

#include <hooker/hooker.hpp>
#include <vulpes/memory/signatures/signatures.hpp>

#include "tick.hpp"

const size_t TICKS_PER_SECOND = 30; //TODO: Move this somewhere else.

DEFINE_EVENT_HOOK_LIST(EVENT_PRE_TICK, pre_events);
DEFINE_EVENT_HOOK_LIST(EVENT_TICK, events);


extern "C" bool before_tick(uint32_t* last_tick_index) {
    call_in_order(pre_events);
    return true;
}

extern "C" void after_tick() {
    call_in_order(events);
}

extern "C" {

    uintptr_t game_tick_actual_jmp;
    extern game_tick_wrapper();

}

static Patch(
    tick_hook_patch, NULL, 7,
    JMP_PATCH, &game_tick_wrapper
);

void init_tick_hook() {
    tick_hook_patch.build(sig_hook_tick());
    tick_hook_patch.apply();
    game_tick_actual_jmp = tick_hook_patch.return_address();
}

void revert_tick_hook() {
    tick_hook_patch.revert();
}
