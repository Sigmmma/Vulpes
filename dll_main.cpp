#include "hooks/hooker.hpp"
#include "fox.h"
//#include "hooks/incoming_packets.hpp" // This is just for a quick test.
#define WIN32_MEAN_AND_LEAN
#include <Windows.h>
#include <WinUser.h>
#include "popout_console/guicon.hpp"
#include <cstdio>

static bool loaded;

struct ImageSectionHeader {
    uint32_t bullshit1[4];
    uint32_t size_of_segment;
    uint32_t offset_to_segment;
    uint32_t bullshit2[4];
};

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    if(fdwReason == DLL_PROCESS_ATTACH && !loaded) {
        loaded = true;
        RedirectIOToConsole();
        printf(_FOX);

        // Get safe search bounds for CodeSignature
        SignatureBounded(true, sig_text_segment_data, 0x400000, 0x401000,
            {0x2E, 0x74, 0x65, 0x78, 0x74, 0x00, 0x00, 0x00});
        ImageSectionHeader* header = reinterpret_cast<ImageSectionHeader*>(sig_text_segment_data.get_address());
        set_lowest_permitted_address(0x400000 + header->offset_to_segment);
        set_highest_permitted_address(0x400000 + header->offset_to_segment + header->size_of_segment);
        printf("Signature search bounds are now %X and %X\n",
            get_lowest_permitted_address(),
            get_highest_permitted_address()
        );

        // Check if the exe is a Server.
        Signature(false, server_sig,
            { 0x75, 0x2D, 0x68, -1, -1, -1, -1, 0xE8, -1, -1, -1, -1, 0x68, -1, -1, -1, -1, 0x68, -1, -1, -1, -1, 0xE8, -1, -1, -1, -1, 0x68, -1, -1, -1, -1, 0x68, -1, -1, -1, -1, 0x33, 0xC0, 0xE8, -1, -1, -1, -1, 0x83, 0xC4, 0x14, 0x8B, 0x44, 0x24, 0x04, 0x68, -1, -1, -1, -1, 0x68, -1, -1, -1, -1, 0x50 });
        uintptr_t is_server = server_sig.get_address();
        if (is_server){
            printf("We're a server\n");
        };
    }
    else if(fdwReason == DLL_PROCESS_DETACH && loaded) {
        loaded = false;
        FreeConsole();
    }
    return true;
}
