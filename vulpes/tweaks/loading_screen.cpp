/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <hooker/hooker.hpp>

#include <vulpes/memory/signatures.hpp>

#include "loading_screen.hpp"

static Patch(loading_screen_background_removal, 0, 5, NOP_PATCH, 0);

void init_loading_screen_fixes() {
    if (loading_screen_background_removal.build(
            sig_loading_screen_background_render_call()))
        loading_screen_background_removal.apply();
}

void revert_loading_screen_fixes() {
    loading_screen_background_removal.revert();
}
