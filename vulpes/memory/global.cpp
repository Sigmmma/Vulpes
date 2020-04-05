/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <vulpes/memory/gamestate/network.hpp>
#include <vulpes/memory/signatures.hpp>

#include "global.hpp"

const char* profile_path() {
    return *sig_path();
}

const char* savegames_path() {
    return *sig_savegames_path();
}

const wchar_t* active_profile_name() {
    return *sig_active_profile_name();
}

int32_t active_profile_id() {
    return **sig_active_profile_id();
}

static const char ui_name[] = "ui";

const char* map_name() {
    const char* name_pointer = NULL;
    if (at_main_menu()){
        name_pointer = &ui_name[0];
    // TODO: Potentially find better way to determine if to return this pointer.
    } else if (*connection_type() == ConnectionType::NONE) {
        name_pointer = *sig_sp_map_name();
    } else {
        name_pointer = *sig_map_name();
    }
    return name_pointer;
}

bool at_main_menu() {
    return *sig_at_main_menu();
}
