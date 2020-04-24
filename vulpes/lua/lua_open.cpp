/*
** Modified from LuaJIT to fit the sandboxed nature of the project.
** Copyright (C) 2019 gbMichelle
**
** Library initialization.
** Copyright (C) 2005-2017 Mike Pall. See Copyright Notice in luajit.h
**
** Major parts taken verbatim from the Lua interpreter.
** Copyright (C) 1994-2012 Lua.org, PUC-Rio. See Copyright Notice at the bottom.
*/

#define lib_init_c
#define LUA_LIB

#include <cctype>
#include <string>

#include <vulpes/functions/messaging.hpp>
#include <vulpes/memory/persistent/global.hpp>
#include <vulpes/paths.hpp>

#include "lua_helpers.hpp"
#include "lua_open.hpp"

// This code has been taken from the LuaJIT source code and modified to fit
// Vulpes needs.

// Sentinel against recursive loops. Taken from LuaJIT lib_package source.
static int sentinel_ = 0;
#define sentinel reinterpret_cast<void *>(&sentinel_)

static int LuaV_sandboxed_require(lua_State *L) {
    // Get the name from the stack:
    auto name = luaL_checkstring(L, 1);
    // LuaJIT source (Added some comments and made an error more clear):
    // Get the table of loaded modules:
    lua_settop(L, 1);
    lua_getfield(L, LUA_REGISTRYINDEX, "_LOADED");
    // Check if the requested name is in the table:
    lua_getfield(L, 2, name);
    if (lua_toboolean(L, -1)) {
        if (lua_touserdata(L, -1) == sentinel) {
            luaL_error(L, "Recursive loop or previous error loading module %s", name);
        }
        return 1;
    }
    // End of LuaJIT source^

    // In sandboxed mode we're not allowing anything but alphanumerics
    // underscores, dashes and dots for file extensions.
    for (char & c : std::string(name)) {
        if ((!isalnum(c)) && (c != '.') && (c != '_') && (c != '-')) {
            char character[2] = {c, 0x00};
            auto error = std::string("Illegal character in sandboxed require statement: \"") + character + "\"";
            return luaL_error(L, error.data());
        }
    }

    // TODO: Load scripts from mapfiles when we have a way to put them in there.
    // Convert the path to a full path.
    auto full_path = std::string(profile_path()) + LUA_MAP_PATH + "\\" + map_name() + "\\" + name;

    if (!is_file(full_path)) {
        return luaL_error(L, "File does not exist or is not a file.");
    }
    // Load the script with its name set to map_name:script_name
    if(luaV_loadfile_as(L, full_path, std::string(map_name()) + ":" + name)) {
        luaV_print_error(L);
        return luaL_error(L, "Something went wrong loading the file.");
    }

    // More LuaJIT source (Original comments):
    if (lua_isfunction(L, -1)) { // did it find module?
        lua_pushlightuserdata(L, sentinel);
        lua_setfield(L, 2, name); // _LOADED[name] = sentinel
        lua_pushstring(L, name); // pass name as argument to module
        lua_call(L, 1, 1); // run loaded module
        if (!lua_isnil(L, -1)) // non-nil return?
            lua_setfield(L, 2, name); // _LOADED[name] = returned value
        lua_getfield(L, 2, name);
        if (lua_touserdata(L, -1) == sentinel) { // module did not set a value?
            lua_pushboolean(L, 1); // use true as result
            lua_pushvalue(L, -1); // extra copy to be returned
            lua_setfield(L, 2, name); // _LOADED[name] = true
        }
    } else {
        // Pop the return value off the stack as it sucks!
        lua_pop(L, 1);
    }
    return 1;
}

LUALIB_API void luaV_openlibs(lua_State *state, bool unlocked) {
    luaL_openlibs(state);

    if (!unlocked) {
        auto sandbox = std::string(
            "dofile = nil\
            load = nil\
            loadfile = nil\
            loadstring = nil\
            os.execute = nil\
            os.exit = nil\
            os.getenv = nil\
            os.remove = nil\
            os.rename = nil\
            os.tmpname = nil\
            io = nil\
            jit = nil\
            debug = nil\
            package = nil\
            require = nil");
        if(luaL_loadbuffer(state, sandbox.data(), sandbox.length(), "sandbox")
        || lua_pcall(state, 0, 0, 0)) {
            luaV_print_error(state);
            return;
        }
        // Register the special sandboxed require statement.
        lua_register(state, "require", LuaV_sandboxed_require);
    }
}

/******************************************************************************
* Copyright (C) 1994-2008 Lua.org, PUC-Rio.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/
