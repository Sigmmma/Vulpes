/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <hooker/hooker.hpp>

#include <vulpes/memory/gamestate/server/king.hpp>

#include "king.hpp"
/*
Signature(true, sig_king_timer_start,
    {0xC7, 0x05, -1, -1, -1, 0x00, 0x08, 0x07, 0x00, 0x00, 0xC7, 0x05,
     -1, -1, -1, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xA3, -1, -1, -1, -1, 0xE8});*/
Signature(true, sig_hill_timer_reset,
    {0xC7, 0x05, -1, -1, -1, 0x00, 0x08, 0x07, 0x00, 0x00,
     0xE8, -1, -1, 0xFF, 0xFF});

extern "C" void king_timer_reset(){
    king_globals()->current_hill_time_left = king_globals_upgrade()->hill_length;
}

extern "C" {
    extern king_timer_reset_hook();
}

/*
Patch(king_timer_start_hook, sig_king_timer_start, 0, 10,
    CALL_PATCH, &king_timer_reset_hook);*/
Patch(king_timer_reset_hook_patch, sig_hill_timer_reset, 0, 10,
    CALL_PATCH, &king_timer_reset_hook);


void init_king_hooks(){
    //if (king_timer_start_hook.build()) = king_timer_start_hook.apply();
    if (king_timer_reset_hook_patch.build()) king_timer_reset_hook_patch.apply();
    KingGlobalsUpgrade* king_upgrade_globals = king_globals_upgrade();
    king_upgrade_globals->hill_length = 60*30;
}

void revert_king_hooks(){
    //king_timer_start_hook.revert();
    king_timer_reset_hook_patch.revert();
}
