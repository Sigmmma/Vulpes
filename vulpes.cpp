#define WIN32_MEAN_AND_LEAN
#include <windows.h>
#include <cstdio>
#include <cstdint>

#include "halo/memory/gamestate/network.hpp"

#include "hooker/hooker.hpp"
#include "halo/hooks/incoming_packets.hpp"
#include "halo/hooks/king.hpp"
#include "halo/hooks/console.hpp"
#include "halo/hooks/tick.hpp"
#include "halo/hooks/object.hpp"
#include "halo/hooks/map.hpp"
void init_hooks(){
    init_incoming_packet_hooks();
    init_king_hooks();
    init_console_hooks();
    init_tick_hook();
    init_object_hooks();
    init_map_hooks(game_is_server_executable());
}
void revert_hooks(){
    revert_incoming_packet_hooks();
    revert_king_hooks();
    revert_console_hooks();
    revert_tick_hook();
    revert_object_hooks();
    revert_map_hooks();
}

#include "command/debug.hpp"
#include "command/server.hpp"
void init_commands(){
    init_debug_commands();
    init_server_commands();
}

#include "halo/fixes/cpu_usage.hpp"
#include "halo/fixes/file_handle_leak.hpp"
#include "halo/fixes/host_refusal.hpp"
#include "halo/fixes/string_overflows.hpp"
#include "halo/fixes/shdr_trans_zfighting.hpp"
#include "halo/fixes/framerate_dependent_timers.hpp"
#include "halo/fixes/animation.hpp"
#include "halo/tweaks/loading_screen.hpp"
#include "halo/tweaks/tweaks.hpp"
void init_halo_bug_fixes(){
    init_cpu_usage_fixes();
    init_file_handle_leak_fixes();
    init_host_refusal_fixes();
    init_string_overflow_fixes();
    ADD_CALLBACK(EVENT_MAP_LOAD_MP, revert_animation_bug_fixes);
    if (!game_is_server_executable()){
        init_framerate_dependent_timer_fixes();
        ADD_CALLBACK(EVENT_MAP_LOAD_SP_UI, init_animation_bug_fixes);
        init_loading_screen_fixes();
        ADD_CALLBACK(EVENT_TICK, init_tweaks);
    };
}

void revert_halo_bug_fixes(){
    revert_cpu_usage_fixes();
    revert_file_handle_leak_fixes();
    revert_host_refusal_fixes();
    revert_string_overflow_fixes();
    revert_shdr_trans_zfighting_fixes();
    revert_framerate_dependent_timer_fixes();
    revert_animation_bug_fixes();
    revert_loading_screen_fixes();
    revert_tweaks();
}

#include "halo/upgrades/map.hpp"
void init_upgrades(){
    init_map_crc_upgrades(game_is_server_executable());
}

void revert_upgrades(){
    revert_map_crc_upgrades();
}

void init_memory(){
}

#include "halo/functions/message_delta.hpp"
void init_halo_functions(){
    init_message_delta_processor();
    init_message_delta_sender();
}


void init_network(){
}

#include "halo/functions/messaging.hpp"
void tell_user_that_we_loaded(){
    cprintf_info("Vulpes has loaded.");
    DEL_CALLBACK(EVENT_TICK, tell_user_that_we_loaded);
}

#include "includes/fox.hpp"
#include "halo/lua/lua.hpp"

SignatureBounded(true, sig_text_segment_data, 0x400000, 0x401000,
    {0x2E, 0x74, 0x65, 0x78, 0x74, 0x00, 0x00, 0x00});
struct ImageSectionHeader {
    uint32_t bullshit1[4];
    uint32_t size_of_segment;
    uint32_t offset_to_segment;
    uint32_t bullshit2[4];
};

void init_vulpes(){
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

    init_lua();

    ADD_CALLBACK(EVENT_TICK, tell_user_that_we_loaded);
}

void destruct_vulpes(){
    revert_hooks();
    revert_halo_bug_fixes();
    revert_upgrades();
    revert_halo_bug_fixes();
}
