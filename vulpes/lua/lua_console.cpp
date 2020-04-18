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

#include <string>

#include <lua.hpp>

#include <util/nanoluadict.hpp>

#include <vulpes/functions/messaging.hpp>

#include "lua_console.hpp"
#include "lua_helpers.hpp"

static int luaV_cprint(lua_State *state) noexcept {
    int num_args = lua_gettop(state);

    std::string combined = "";
    // Arguments from the lua state start at index 1.
    for (size_t i = 1; i <= num_args; i++) {
         combined += luaV_check_as_string(state, i) + " ";
    }

    cprintf("%s", combined.data());
    return 0;
}

static int luaV_console_clear(lua_State *state){
    if (lua_gettop(state)) {
        luaL_error(state, "console.clear does not take any args.");
    } else {
        console_clear();
    }
    return 0;
}

void luaV_register_console(lua_State *state) {
    luaDict(state,
        "console",
        2,
        kvPairWithCFunction("print", luaV_cprint),
        kvPairWithCFunction("clear", luaV_console_clear)
    );
}
