#
# Vulpes (c) 2019 gbMichelle
#
# This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
#

cmake_minimum_required(VERSION 3.10)

# Project name
project(Vulpes
    VERSION 0.0.0
    DESCRIPTION "Opensource Halo Client and Server Extension Mod"
    LANGUAGES C CXX ASM
)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-value -Wold-style-cast -masm=intel -msse -msse2 -m32 -mfpmath=sse")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O3")
set(CMAKE_SHARED_LINKER_FLAGS "-m32 -W -s -static-libgcc -static-libstdc++ -lkernel32")


add_library(Vulpes SHARED
    vulpes/version.rc
    vulpes/vulpes.cpp

    vulpes/command/debug.cpp
    vulpes/command/handler.cpp
    vulpes/command/server.cpp

    hooker/hooker.cpp

    includes/string_raw_data_encoder.c
    includes/crc32.c

    vulpes/fixes/animation.cpp
    vulpes/fixes/animation.S
    vulpes/fixes/cpu_usage.cpp
    vulpes/fixes/cpu_usage.S
    vulpes/fixes/file_handle_leak.cpp
    vulpes/fixes/framerate_dependent_timers.cpp
    vulpes/fixes/host_refusal.cpp
    vulpes/fixes/shdr_trans_zfighting.cpp
    vulpes/fixes/string_overflows.cpp

    vulpes/functions/devmode.cpp
    vulpes/functions/message_delta.cpp
    vulpes/functions/messaging.cpp
    vulpes/functions/object_unit.cpp
    vulpes/functions/table.cpp

    vulpes/hooks/console.cpp
    vulpes/hooks/console.S
    vulpes/hooks/incoming_packets.cpp
    vulpes/hooks/incoming_packets.S
    vulpes/hooks/king.cpp
    vulpes/hooks/king.S
    vulpes/hooks/map.cpp
    vulpes/hooks/map.S
    vulpes/hooks/object.cpp
    vulpes/hooks/object.S
    vulpes/hooks/tick.cpp
    vulpes/hooks/tick.S

    vulpes/memory/gamestate/object/object.cpp
    vulpes/memory/gamestate/object/object_device.cpp
    vulpes/memory/gamestate/object/object_item.cpp
    vulpes/memory/gamestate/object/object_unit.cpp
    vulpes/memory/gamestate/server/game_variant.cpp
    vulpes/memory/gamestate/server/king.cpp
    vulpes/memory/gamestate/console.cpp
    vulpes/memory/gamestate/effect.cpp
    vulpes/memory/gamestate/network.cpp
    vulpes/memory/gamestate/player.cpp
    vulpes/memory/gamestate/table.cpp
    vulpes/memory/behavior_definition.cpp
    vulpes/memory/global.cpp

    vulpes/memory/types.cpp

    vulpes/network/foxnet/vulpes_message.cpp
    vulpes/network/network_id.cpp

    vulpes/tweaks/loading_screen.cpp
    vulpes/tweaks/tweaks.cpp

    vulpes/upgrades/map.cpp
    vulpes/upgrades/map.S

    vulpes/lua/lua.cpp
    vulpes/lua/lua_messaging.cpp
    vulpes/lua/lua_open.cpp
)

add_library(VulpesLoader SHARED
    loader/version.rc
    loader/dll_main.c
)

ADD_LIBRARY(LuaJIT STATIC IMPORTED)
SET_TARGET_PROPERTIES(LuaJIT PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/LuaJIT/src/libluajit.a")
set(LUA_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/LuaJIT/src")

target_include_directories(Vulpes
    PUBLIC ${LUA_INCLUDE_DIR}
)
TARGET_LINK_LIBRARIES(Vulpes LuaJIT)

SET_TARGET_PROPERTIES(Vulpes PROPERTIES PREFIX "")
SET_TARGET_PROPERTIES(VulpesLoader PROPERTIES PREFIX "")
