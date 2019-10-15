#include <string>

#include <lua.hpp>

// Call this when a lua frame encounters an error.
static inline void luaV_print_error(lua_State *state){
    // Print whatever is at the top of the lua stack.
    // This should be an error when this function is called.
    cprintf_error(lua_tostring(state, -1));
    // TODO: Make errors log to a file.
    lua_pop(state, 1);
}

static inline std::string luaV_check_as_string(lua_State *state, int stackpos){
    if(lua_isboolean(state, stackpos)){
        return std::string(lua_toboolean(state, stackpos) ? "true" : "false");
    }else if(lua_isnil(state, stackpos)){
        return std::string("nil");
    }else{
        return std::string(luaL_checkstring(state, stackpos));
    };
}
