/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
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
    for (size_t i = 1; i <= num_args; i++){
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
