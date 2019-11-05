/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <hooker/hooker.hpp>

#include "loading_screen.hpp"

Signature(false, sig_loading_screen_background_render_call,
    {0xE8, -1, -1, -1, -1, 0x83, 0xC4, 0x0C, 0x55, 0x6A, 0x02, 0x6A, 0xFF});

Patch(loading_screen_background_removal,
    sig_loading_screen_background_render_call, 0, 5, NOP_PATCH, 0);

void init_loading_screen_fixes(){
    if (loading_screen_background_removal.build()) loading_screen_background_removal.apply();
}

void revert_loading_screen_fixes(){
    loading_screen_background_removal.revert();
}
