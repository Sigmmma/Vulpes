/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include "tick.hpp"
#include "../../hooker/hooker.hpp"
#include "../functions/messaging.hpp"
#include "../memory/types.hpp"
#include <chrono>

bool print_tick_duration = false;

const size_t TICKS_PER_SECOND = 30; //TODO: Move this somewhere else.

DEFINE_EVENT_HOOK_LIST(EVENT_PRE_TICK, pre_events);
DEFINE_EVENT_HOOK_LIST(EVENT_TICK, events);

static auto before = std::chrono::high_resolution_clock::now();

static float tick_duration[TICKS_PER_SECOND];
static size_t tick_count = 0;

extern "C" bool before_tick(uint32_t* last_tick_index){
    before = std::chrono::high_resolution_clock::now();
    call_in_order(pre_events);
    return true;
}

extern "C" void after_tick(){
    call_in_order(events);

    // Benchmarking stuff.
    // Move this somewhere else eventually.

    auto after = std::chrono::high_resolution_clock::now();
    int mod = tick_count % TICKS_PER_SECOND;
    tick_duration[mod] = static_cast<float>(std::chrono::duration_cast<std::chrono::nanoseconds>(after-before).count());
    if (print_tick_duration && tick_count > 0 && mod == 0){
        float average = avg<float>(tick_duration, TICKS_PER_SECOND);
        cprintf(
            "Average Tick Time for last second: %.6f ms",
            average / 1000000.0 // Nanoseconds to miliseconds.
        );
    };
    tick_count++;
}

Signature(true, sig_tick,
    {0x51, 0x53, 0x68, 0xFF, 0xFF, 0x0F, 0x00, 0x68, 0x1F, 0x00, 0x09,
     0x00, 0xC6, 0x05, -1, -1, -1, -1, 0x01});

extern "C" {

    uintptr_t game_tick_actual_jmp;
    extern game_tick_wrapper();

}

Patch(
    tick_hook_patch,
    sig_tick, 0, 7,
    JMP_PATCH, &game_tick_wrapper
);

void init_tick_hook(){
    tick_hook_patch.build();
    tick_hook_patch.apply();
    game_tick_actual_jmp = tick_hook_patch.return_address();
}

void revert_tick_hook(){
    tick_hook_patch.revert();
}
