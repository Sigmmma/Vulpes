/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include "lua.hpp"
#include "../memory/types.hpp"
#include "../memory/global.hpp"

#define WIN32_MEAN_AND_LEAN
#include <windows.h>
#include <memory>
#include <cstdio>
#include <string>
#include <regex>

#include "../paths.hpp"
#include "lua_messaging.hpp"
#include "../../includes/file_helpers.hpp"
#include "../hooks/map.hpp"
#include "../functions/messaging.hpp"
#include "../memory/global.hpp"

#include <lua.hpp>

static bool initialized = false;

std::regex lua_finder("\\.lua$", std::regex::icase);

bool is_lua_file(std::string path){
    if (is_file(path.data())){
        return (!!std::regex_match(path, lua_finder));
    }
    return false;
}

void load_map_lua_scripts(){
    cprintf_info("Attempting to find a lua script.");

    char path_string[4096+1];
    sprintf_s(path_string, 4096, LUA_MAP_PATH "\\%s", profile_path(), map_name());

    if (is_dir(path_string)){
        cprintf_info("Found a folder!");
        char query_string[4096+1];
        sprintf_s(query_string, 4096, "%s\\*.lua", path_string);
        WIN32_FIND_DATA find_file_data;
        HANDLE handle = FindFirstFile(query_string, &find_file_data);
        if (handle != INVALID_HANDLE_VALUE){
            char file_string[4096+1];
            sprintf_s(file_string, 4096, "%s\\%s", path_string, find_file_data.cFileName);
            cprintf_info(file_string);
            FILE *f = fopen(file_string, "rb+");
            if (f){
                size_t size = file_get_size(f);
                char contents[size];
                file_read_into_buffer(contents, f);
                fclose(f);

                auto *state = luaL_newstate();

                register_messaging_functions(state);

                if(luaL_loadstring(state, contents)
                || lua_pcall(state, 0, 0, 0)){
                    cprintf_error("Failed to load lua script. %s", lua_tostring(state, -1));
                    lua_pop(state, 1);
                    lua_close(state);
                    return;
                }

            };
        };
        return;
    };

    cprintf_info("There was no folder with the corresponding map name.");
}

void init_lua(){
    initialized = true;
    { // Create folders.
        char path_string[4096+1];
        sprintf_s(path_string, 4096, VULPES_PATH,     profile_path());
        CreateDirectory(path_string, NULL);
        sprintf_s(path_string, 4096, LUA_BASE_PATH,   profile_path());
        CreateDirectory(path_string, NULL);
        sprintf_s(path_string, 4096, LUA_MAP_PATH,    profile_path());
        CreateDirectory(path_string, NULL);
        sprintf_s(path_string, 4096, LUA_GLOBAL_PATH, profile_path());
        CreateDirectory(path_string, NULL);
    }
    // init_callbacks();
    ADD_CALLBACK(EVENT_MAP_LOAD, load_map_lua_scripts);
}

void destruct_lua(){

    initialized = false;
}
