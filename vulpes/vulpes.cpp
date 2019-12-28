/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <cstdint>
#include <cstdio>
#include <windows.h>

#include "memory/gamestate/network.hpp"

// Hooks

#include "hooks/incoming_packets.hpp"
#include "hooks/king.hpp"
#include "hooks/console.hpp"
#include "hooks/tick.hpp"
#include "hooks/object.hpp"
#include "hooks/map.hpp"
void init_hooks() {
    init_incoming_packet_hooks();
    init_king_hooks();
    init_console_hooks();
    init_tick_hook();
    init_object_hooks();
    init_map_hooks(game_is_server_executable());
}
void revert_hooks() {
    revert_incoming_packet_hooks();
    revert_king_hooks();
    revert_console_hooks();
    revert_tick_hook();
    revert_object_hooks();
    revert_map_hooks();
}

// Commands

#include "command/debug.hpp"
#include "command/server.hpp"
void init_commands() {
    init_debug_commands();
    init_server_commands();
}

// Fixes

#include "fixes/cpu_usage.hpp"
#include "fixes/host_refusal.hpp"
#include "fixes/string_overflows.hpp"
#include "fixes/shdr_trans_zfighting.hpp"
#include "fixes/framerate_dependent_timers.hpp"
#include "fixes/animation.hpp"
#include "tweaks/loading_screen.hpp"
#include "tweaks/tweaks.hpp"
void init_halo_bug_fixes() {
    init_cpu_usage_fixes();
    init_host_refusal_fixes();
    init_string_overflow_fixes();
    ADD_CALLBACK(EVENT_MAP_LOAD_MP, revert_animation_bug_fixes);
    if (!game_is_server_executable()) {
        init_framerate_dependent_timer_fixes();
        ADD_CALLBACK(EVENT_MAP_LOAD_SP_UI, init_animation_bug_fixes);
        init_loading_screen_fixes();
        ADD_CALLBACK(EVENT_TICK, init_tweaks);
    }
}

void revert_halo_bug_fixes() {
    revert_cpu_usage_fixes();
    revert_host_refusal_fixes();
    revert_string_overflow_fixes();
    revert_shdr_trans_zfighting_fixes();
    revert_framerate_dependent_timer_fixes();
    revert_animation_bug_fixes();
    revert_loading_screen_fixes();
    revert_tweaks();
}

// Upgrades

#include "upgrades/map.hpp"
void init_upgrades() {
    init_map_crc_upgrades(game_is_server_executable());
}

void revert_upgrades() {
    revert_map_crc_upgrades();
}

// Memory

void init_memory() {
}

// Halo functions

#include "functions/message_delta.hpp"
void init_halo_functions() {
    init_message_delta_processor();
    init_message_delta_sender();
}

#include "network/network_id.hpp"
void init_network() {
    init_network_id();
}

// Main init.

#include <util/fox.hpp>
#include <hooker/hooker.hpp>

#include "functions/messaging.hpp"
#include "lua/lua.hpp"

void pre_first_map_load_init();

SignatureBounded(true, sig_text_segment_data, 0x400000, 0x401000,
    {0x2E, 0x74, 0x65, 0x78, 0x74, 0x00, 0x00, 0x00});
struct ImageSectionHeader {
    uint32_t bullshit1[4];
    uint32_t size_of_segment;
    uint32_t offset_to_segment;
    uint32_t bullshit2[4];
};

void init_vulpes() {
    // Get safe search bounds for CodeSignature.
    ImageSectionHeader* header = reinterpret_cast<ImageSectionHeader*>(sig_text_segment_data.address());
    set_lowest_permitted_address(0x400000 + header->offset_to_segment);
    set_highest_permitted_address(0x400000 + header->offset_to_segment + header->size_of_segment);

    printf(_FOX);

    init_hooks();
    init_halo_bug_fixes();
    init_upgrades();
    init_halo_functions();
    init_network();
    init_commands();


    // Final initialization step for things that act on data that isn't valid
    // until way later when the game has loaded more.
    ADD_CALLBACK(EVENT_PRE_MAP_LOAD, pre_first_map_load_init);
}

void pre_first_map_load_init() {
    DEL_CALLBACK(EVENT_PRE_MAP_LOAD, pre_first_map_load_init);

    // Initialize lua.
    // TODO: Potentially find better place for this that will also load on the
    // server if the server has no decided to load a map yet.
    init_lua();

    // Tell the user that we loaded, so it is less ambiguous.
    cprintf_info("Vulpes loaded.");
}

void destruct_vulpes() {
    revert_hooks();
    revert_halo_bug_fixes();
    revert_upgrades();
    revert_halo_bug_fixes();
    destruct_lua();
}

// DLL Entrypoint.

bool loaded = false;

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    if(fdwReason == DLL_PROCESS_ATTACH && !loaded) {
        loaded = true;
        init_vulpes();
    } else if(fdwReason == DLL_PROCESS_DETACH && loaded) {
        destruct_vulpes();
        loaded = false;
    }
    return true;
}
