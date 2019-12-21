/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
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

Patch(king_timer_reset_hook_patch, 0, 10,
    CALL_PATCH, &king_timer_reset_hook);

void init_king_hooks() {
    king_timer_reset_hook_patch.build(hook_hill_timer_reset());
    king_timer_reset_hook_patch.apply();
    KingGlobalsUpgrade* king_upgrade_globals = king_globals_upgrade();
    king_upgrade_globals->hill_length = 60*30;
}

void revert_king_hooks() {
    king_timer_reset_hook_patch.revert();
}
