/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#pragma once

// This is a wrapper for lua_open that replaces certain functions,
// and excludes others for the safety of the user.

#include <lua.hpp>

LUALIB_API void luaV_openlibs(lua_State *state, bool unlocked);
