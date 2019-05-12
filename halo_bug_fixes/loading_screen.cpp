#include "loading_screen.hpp"
#include "../hooks/hooker.hpp"

Signature(false, sig_loading_screen_background_render_call,
    {0xE8, -1, -1, -1, -1, 0x83, 0xC4, 0x0C, 0x55, 0x6A, 0x02, 0x6A, 0xFF});

Patch(loading_screen_background_removal);

void init_loading_screen_fixes(){
    static uintptr_t sig_addr = sig_loading_screen_background_render_call.get_address();
    if (!loading_screen_background_removal.is_built()){
        loading_screen_background_removal.build(sig_addr, 5, NOP_PATCH, 0);
    };
    if (loading_screen_background_removal.is_built()){
        loading_screen_background_removal.apply();
    };
}

void revert_loading_screen_fixes(){
    loading_screen_background_removal.revert();
}
