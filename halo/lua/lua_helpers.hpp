#pragma once

#include <cstdio>
#include <string>
#include <memory>

#include <lua.hpp>

#include "../../includes/file_helpers.hpp"
#include "../functions/messaging.hpp"

// Call this when a lua frame encounters an error.
static inline void luaV_print_error(lua_State *state){
    // Print whatever is at the top of the lua stack.
    // This should be an error when this function is called.
    auto text = lua_tostring(state, -1);
    cprintf_error("%s", text);
    printf("%s", text);
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

static inline int luaV_loadfile_as(lua_State *state, std::string path, std::string name){
    // Check if file exists.
    // TODO: Make this a proper error.
    if (!is_file(path)) return 0;

    FILE* lua_file = fopen(path.data(), "r");
    if (!lua_file) return 0;
    size_t size = file_get_size(lua_file);
    auto content = std::make_unique<char[]>(size);
    file_read_into_buffer(content.get(), lua_file);
    fclose(lua_file);
    // Load contents of file into the lua compiler
    return luaL_loadbuffer(state, content.get(), size, name.data());
}
