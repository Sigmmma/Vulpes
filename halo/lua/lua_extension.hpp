#include <string>

#include <lua.hpp>

static inline std::string luaV_check_as_string(lua_State *state, int stackpos){
    if(lua_isboolean(state, stackpos)){
        return std::string(lua_toboolean(state, stackpos) ? "true" : "false");
    }else if(lua_isnil(state, stackpos)){
        return std::string("nil");
    }else{
        return std::string(luaL_checkstring(state, stackpos));
    };
}
