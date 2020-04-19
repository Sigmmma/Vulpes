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

#include <cstdio>
#include <memory>
#include <string>

#include <lua.hpp>

#include <util/file_helpers.hpp>

#include <vulpes/functions/messaging.hpp>

// Call this when a lua frame encounters an error.
static inline void luaV_print_error(lua_State *state) {
    // Print whatever is at the top of the lua stack.
    // This should be an error when this function is called.
    auto text = lua_tostring(state, -1);
    cprintf_error("%s", text);
    printf("%s", text);
    // TODO: Make errors log to a file.
    lua_pop(state, 1);
}

// Don't use this function if your input arg really needs to be a string.
// This is just to get a string representation of whatever basic datatype is
// being read from the stack.
static inline std::string luaV_check_as_string(lua_State *state, int stackpos) {
    if(lua_isboolean(state, stackpos)) {
        return std::string(lua_toboolean(state, stackpos) ? "true" : "false");
    } else if(lua_isnil(state, stackpos)) {
        return std::string("nil");
    } else {
        return std::string(luaL_checkstring(state, stackpos));
    }
}

// Like LuaL_loadfile but it lets you specify a name for the error messages.
static inline int luaV_loadfile_as(lua_State *state, std::string path, std::string name) {
    FILE* lua_file = fopen(path.data(), "r");
    // Can't load file, positive return value in line with existing lua c convention.
    if (!lua_file) return 1;
    // Load file into memory.
    size_t size = file_get_size(lua_file);
    auto content = std::make_unique<char[]>(size);
    file_read_into_buffer(content.get(), lua_file);
    // Close the file.
    fclose(lua_file);
    // Load contents of file into the lua compiler
    return luaL_loadbuffer(state, content.get(), size, name.data());
}
