#include "lua_messaging.hpp"

#include "../functions/messaging.hpp"

static int lua_cprint(lua_State *state) noexcept {
    int nargs = lua_gettop(state);
    if(nargs == 1 || nargs == 4 || nargs == 5) {
        const char *text;

        if(lua_isboolean(state,1)){
            text = lua_toboolean(state,1) ? "true" : "false";
        }else if(lua_isnil(state,1)){
            text = "(nil)";
        }else{
            text = luaL_checkstring(state,1);
        };
        if(nargs >= 4) {
            ARGBFloat argb;
            if(nargs == 5) {
                argb.alpha = luaL_checknumber(state,2);
                argb.red   = luaL_checknumber(state,3);
                argb.green = luaL_checknumber(state,4);
                argb.blue  = luaL_checknumber(state,5);
            }else{
                argb.alpha = 1.0;
                argb.red   = luaL_checknumber(state,2);
                argb.green = luaL_checknumber(state,3);
                argb.blue  = luaL_checknumber(state,4);
            }
            cprintf(argb, "%s", text);
        }else{
            cprintf("%s", text);
        }
        return 0;
    }
    else return luaL_error(state,"wrong number of arguments used for cprint()");
}

void register_messaging_functions(lua_State *state){
    lua_register(state, "cprint", lua_cprint);
}
