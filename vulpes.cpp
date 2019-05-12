#define WIN32_MEAN_AND_LEAN
#include <Windows.h>
#include <cstdio>
#include <cstdint>

static bool server = false;

#include "hooks/hooker.hpp"
#include "hooks/incoming_packets.hpp"
#include "hooks/console.hpp"
#include "hooks/tick.hpp"
#include "hooks/map.hpp"
void init_hooks(){
    init_code_caves();

    init_incoming_packet_hooks();
    init_console_hooks();
    init_tick_hook();
    init_map_hooks();
}
void revert_hooks(){
    revert_code_caves();

    revert_incoming_packet_hooks();
    revert_console_hooks();
    revert_tick_hook();
    revert_map_hooks();
}

#include "command/debug.hpp"
void init_commands(){
    init_debug_commands();
}

#include "halo_bug_fixes/cpu_usage.hpp"
#include "halo_bug_fixes/file_handle_leak.hpp"
#include "halo_bug_fixes/host_refusal.hpp"
#include "halo_bug_fixes/string_overflows.hpp"
#include "halo_bug_fixes/shdr_trans_zfighting.hpp"
#include "halo_bug_fixes/framerate_dependent_timers.hpp"
#include "halo_bug_fixes/animation_bugs.hpp"
#include "halo_bug_fixes/loading_screen.hpp"
void init_halo_bug_fixes(){
    init_cpu_usage_fixes();
    init_file_handle_leak_fixes();
    init_host_refusal_fixes();
    init_string_overflow_fixes();
    ADD_EVENT(EVENT_MAP_LOAD_MP, revert_animation_bug_fixes_e);
    if (!server){
        init_framerate_dependent_timer_fixes();
        ADD_EVENT(EVENT_MAP_LOAD_SP_UI, init_animation_bug_fixes_e);
        init_loading_screen_fixes();
    }
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
}

#include "memory/table.hpp"
void init_memory(){
    init_tables();
}

void init_halo_functions(){
}

//#include "network/message_delta/definition.hpp"
//#include "network/message_delta/bitstream.hpp"
#include "network/message_delta/message_delta_processor.hpp"
#include "network/message_delta/message_delta_sender.hpp"
void init_network(){
    //init_new_definitions();
    //init_bitstream();
    init_message_delta_processor();
    init_message_delta_sender();
}

#include "fox.h"
#include "popout_console/guicon.hpp"

static SignatureBounded(true, sig_text_segment_data, 0x400000, 0x401000,
    {0x2E, 0x74, 0x65, 0x78, 0x74, 0x00, 0x00, 0x00});
static Signature(false, sig_server,
    { 0x75, 0x2D, 0x68, -1, -1, -1, -1, 0xE8, -1, -1, -1, -1, 0x68, -1, -1, -1, -1, 0x68, -1, -1, -1, -1, 0xE8, -1, -1, -1, -1, 0x68, -1, -1, -1, -1, 0x68, -1, -1, -1, -1, 0x33, 0xC0, 0xE8, -1, -1, -1, -1, 0x83, 0xC4, 0x14, 0x8B, 0x44, 0x24, 0x04, 0x68, -1, -1, -1, -1, 0x68, -1, -1, -1, -1, 0x50 });
struct ImageSectionHeader {
    uint32_t bullshit1[4];
    uint32_t size_of_segment;
    uint32_t offset_to_segment;
    uint32_t bullshit2[4];
};

void init_vulpes(){
    // Get safe search bounds for CodeSignature.
    ImageSectionHeader* header = reinterpret_cast<ImageSectionHeader*>(sig_text_segment_data.get_address());
    set_lowest_permitted_address(0x400000 + header->offset_to_segment);
    set_highest_permitted_address(0x400000 + header->offset_to_segment + header->size_of_segment);
    // Check if we're a server.
    server = sig_server.get_address() != 0;
    if(!server){
        RedirectIOToConsole();
    };
    printf(_FOX);

    init_hooks();
    init_halo_bug_fixes();
    init_halo_functions();
    //init_memory();
    init_network();
    init_commands();
}

void destruct_vulpes(){
    if (!server){
        FreeConsole();
    };

    revert_hooks();
    revert_halo_bug_fixes();
}
