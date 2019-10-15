/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#pragma once

#include <lua.hpp>

// Call this when a lua frame encounters an error.
void print_lua_error(lua_State *state);

void init_lua();
void destruct_lua();
