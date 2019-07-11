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
#include <cstdio>

static bool initialized = false;

void init_lua(){
    initialized = true;
    { // Create folders.
        char path_string[4096];
        sprintf(path_string,"%s\\vulpes", profile_path());
        CreateDirectory(path_string, NULL);
        sprintf(path_string,"%s\\vulpes\\lua", profile_path());
        CreateDirectory(path_string, NULL);
        sprintf(path_string,"%s\\vulpes\\lua\\map", profile_path());
        CreateDirectory(path_string, NULL);
        sprintf(path_string,"%s\\vulpes\\lua\\global", profile_path());
        CreateDirectory(path_string, NULL);
    }
    // init_callbacks();

}

void destruct_lua(){

    initialized = false;
}
