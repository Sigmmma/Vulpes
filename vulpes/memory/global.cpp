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

uintptr_t saved_game_file_get_path_to_enclosing_directory_ptr;

void init_memory_global() {
    saved_game_file_get_path_to_enclosing_directory_ptr = sig_saved_game_file_get_path_to_enclosing_directory();
}
