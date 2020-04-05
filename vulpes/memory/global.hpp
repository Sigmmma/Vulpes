/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#pragma once

const char* profile_path();
const char* savegames_path();
const wchar_t* active_profile_name();
/* Returns -1 if not found */
int32_t active_profile_id();
const char* map_name();
bool at_main_menu();
