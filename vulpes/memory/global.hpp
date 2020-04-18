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

#pragma once

const char* profile_path();
const char* savegames_path();
const wchar_t* active_profile_name();
/* Returns -1 if not found */
int32_t active_profile_id();
const char* map_name();
bool at_main_menu();

extern "C" __attribute__((regparm(2)))
/* Writes the path to the requested profile's save folder to the given pointer */
void saved_game_file_get_path_to_enclosing_directory(uint32_t profile_id, char* write_to);


void init_memory_global();
