/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <string>
#define WIN32_MEAN_AND_LEAN
#include <windows.h>

#include "../../includes/file_helpers.hpp"
#include "../functions/messaging.hpp"
#include "../hooks/map.hpp"
#include "../memory/global.hpp"
#include "../memory/types.hpp"
#include "../paths.hpp"

#include "lua_messaging.hpp"
#include "lua.hpp"

static bool initialized = false;

void print_lua_error(lua_State *state){
    // Print whatever is at the top of the lua stack.
    // This should be an error when this function is called.
    cprintf_error(lua_tostring(state, -1));
    // TODO: Make errors log to a file.
    lua_pop(state, 1);
}

void load_map_lua_scripts(){
    // Get path to where the lua scripts should be stored.
    auto path_str = std::string(profile_path()) + LUA_MAP_PATH + "\\" + map_name() + "\\";

    // Check if it is actually a directory / even exists.
    if (is_dir(path_str)){
        // Create a query for paths that match the lua filetype.
        auto query_str = path_str + "\\*.lua";
        WIN32_FIND_DATA find_file_data;
        FindFirstFile(query_str.data(), &find_file_data);

        // If it's actually a file.
        auto file_str = path_str + find_file_data.cFileName;
        if (is_file(file_str)){
            // Create a new lua state.
            auto *state = luaL_newstate();

            // Register funcions.
            register_messaging_functions(state);

            // Load contents of file into the lua compiler
            if(luaL_loadfile(state, file_str.data())
            || lua_pcall(state, 0, 0, 0)){
                print_lua_error(state);
                lua_close(state);
                return;
            };
        };
        return;
    };

    cprintf_info("There was no folder with the corresponding map name.");
}

void init_lua(){
    initialized = true;
    { // Create folders.
        make_dir(std::string(profile_path()) + VULPES_PATH);
        make_dir(std::string(profile_path()) + LUA_BASE_PATH);
        make_dir(std::string(profile_path()) + LUA_MAP_PATH);
        make_dir(std::string(profile_path()) + LUA_GLOBAL_PATH);
    }
    // init_callbacks();
    ADD_CALLBACK(EVENT_MAP_LOAD, load_map_lua_scripts);
}

void destruct_lua(){

    initialized = false;
}
