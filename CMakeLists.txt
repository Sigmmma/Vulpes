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

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-value -Wold-style-cast -masm=intel -msse2 -m32 -mfpmath=sse")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O3")
set(CMAKE_SHARED_LINKER_FLAGS "-m32 -W -s -static-libgcc -static-libstdc++ -lkernel32")


add_library(Vulpes SHARED
    version.rc
    dll_main.cpp
    vulpes.cpp

    command/debug.cpp
    command/handler.cpp
    command/server.cpp

    hooker/code_cave.cpp
    hooker/hooker.cpp

    includes/string_raw_data_encoder.c
    includes/crc32.c

    halo/fixes/animation.cpp
    halo/fixes/animation.s
    halo/fixes/cpu_usage.cpp
    halo/fixes/cpu_usage.s
    halo/fixes/file_handle_leak.cpp
    halo/fixes/framerate_dependent_timers.cpp
    halo/fixes/host_refusal.cpp
    halo/fixes/shdr_trans_zfighting.cpp
    halo/fixes/string_overflows.cpp

    halo/functions/devmode.cpp
    halo/functions/message_delta.cpp
    halo/functions/messaging.cpp
    halo/functions/object_unit.cpp
    halo/functions/table.cpp

    halo/hooks/console.cpp
    halo/hooks/console.s
    halo/hooks/incoming_packets.cpp
    halo/hooks/incoming_packets.s
    halo/hooks/king.cpp
    halo/hooks/king.s
    halo/hooks/map.cpp
    halo/hooks/object.cpp
    halo/hooks/object.s
    halo/hooks/tick.cpp

    halo/memory/gamestate/object/object.cpp
    halo/memory/gamestate/object/object_device.cpp
    halo/memory/gamestate/object/object_item.cpp
    halo/memory/gamestate/object/object_unit.cpp
    halo/memory/gamestate/server/game_variant.cpp
    halo/memory/gamestate/server/king.cpp
    halo/memory/gamestate/console.cpp
    halo/memory/gamestate/effect.cpp
    halo/memory/gamestate/network.cpp
    halo/memory/gamestate/player.cpp
    halo/memory/gamestate/table.cpp
    halo/memory/behavior_definition.cpp
    halo/memory/global.cpp

    halo/memory/types.cpp

    halo/network/foxnet/vulpes_message.cpp

    halo/tweaks/loading_screen.cpp
    halo/tweaks/tweaks.cpp

    halo/upgrades/map.cpp
    halo/upgrades/map.s

    halo/lua/lua.cpp
)

add_library(VulpesLoader SHARED
    loader/version.rc
    loader/dll_main.c
)

SET_TARGET_PROPERTIES(Vulpes PROPERTIES PREFIX "")
SET_TARGET_PROPERTIES(VulpesLoader PROPERTIES PREFIX "")
