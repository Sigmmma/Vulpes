#include "tweaks.hpp"
#include "../../hooker/hooker.hpp"
#include "../hooks/tick.hpp"

Signature(false, sig_console_enabled,
    {0xA0, -1, -1, -1, -1, 0x84, 0xC0, 0x74, 0x28});

bool* console_enabled;

void enable_console(){
    *console_enabled = true;
    DEL_CALLBACK(EVENT_TICK, enable_console);
}

void init_tweaks(){
    static intptr_t sig_addr1 = sig_console_enabled.address();
    if (sig_addr1){
        console_enabled = *(bool**)(sig_addr1+1);
        ADD_CALLBACK(EVENT_TICK, enable_console);
    };
}

void revert_tweaks(){
}
