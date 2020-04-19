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

#include <vulpes/memory/gamestate/server/king.hpp>
#include <vulpes/memory/signatures.hpp>

#include "king.hpp"

extern "C" void king_timer_reset() {
    king_globals()->current_hill_time_left =
        king_globals_upgrade()->hill_length;
}

extern "C" {
    extern king_timer_reset_hook();
}

static Patch(king_timer_reset_hook_patch, NULL, 10,
    CALL_PATCH, &king_timer_reset_hook);

void init_king_hooks() {
    king_timer_reset_hook_patch.build(sig_hook_hill_timer_reset());
    king_timer_reset_hook_patch.apply();
    KingGlobalsUpgrade* king_upgrade_globals = king_globals_upgrade();
    king_upgrade_globals->hill_length = 60*30;
}

void revert_king_hooks() {
    king_timer_reset_hook_patch.revert();
}
