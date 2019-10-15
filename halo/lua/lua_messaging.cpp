#include "lua_messaging.hpp"

#include <lua.hpp>

#include "../functions/messaging.hpp"
#include "lua_helpers.hpp"

static int luaV_cprint(lua_State *state) noexcept {
    // Check if the number of arguments is correct for a print call.
    if(lua_gettop(state) == 1) {
        cprintf("%s", luaV_check_as_string(state, 1).data());
        return 0;
    }
    else return luaL_error(state,"wrong number of arguments used for cprint()");
}

void luaV_reg_messaging_funcs(lua_State *state){
    lua_register(state, "cprint", luaV_cprint);
}
