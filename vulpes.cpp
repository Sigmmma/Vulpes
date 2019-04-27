#define WIN32_MEAN_AND_LEAN
#include <Windows.h>
#include <cstdio>
#include <cstdint>

static bool server;

#include "hooks/hooker.hpp"
#include "hooks/incoming_packets.hpp"
#include "hooks/console.hpp"
void init_hooks(){
    init_incoming_packet_hooks();
    init_console_hooks();
}
void revert_hooks(){
    revert_incoming_packet_hooks();
    revert_console_hooks();
}

#include "halo_bug_fixes/cpu_usage.hpp"
#include "halo_bug_fixes/file_handle_leak.hpp"
#include "halo_bug_fixes/host_refusal.hpp"
#include "halo_bug_fixes/string_overflows.hpp"
void init_halo_bug_fixes(){
    init_cpu_usage_fixes();
    init_file_handle_leak_fixes();
    init_host_refusal_fixes();
    init_string_overflow_fixes();
}

void revert_halo_bug_fixes(){
    revert_cpu_usage_fixes();
    revert_file_handle_leak_fixes();
    revert_host_refusal_fixes();
    revert_string_overflow_fixes();
}

#include "memory/table.hpp"
void init_memory(){
    init_tables();
}

void init_halo_functions(){
}

#include "network/message_delta/definition.hpp"
#include "network/message_delta/bitstream.hpp"

void init_network(){
    init_new_definitions();
    init_bitstream();
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
}

void destruct_vulpes(){
    if (!server){
        FreeConsole();
    };

    revert_hooks();
    revert_halo_bug_fixes();
}
