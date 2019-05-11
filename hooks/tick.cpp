#include "tick.hpp"
#include "hooker.hpp"

static std::vector<Event<EVENT_PRE_TICK>> pre_events;
static std::vector<Event<EVENT_TICK>> events;
std::vector<Event<EVENT_PRE_TICK>>* EVENT_PRE_TICK_list(){
    return &pre_events;
}
std::vector<Event<EVENT_TICK>>* EVENT_TICK_list(){
    return &events;
}

bool before_tick(intptr_t original_args_ptr){
    call_in_order(pre_events);
    return true;
}

void after_tick(intptr_t original_args_ptr){
    call_in_order(events);
}

Signature(true, sig_tick,
    {0x51, 0x53, 0x68, 0xFF, 0xFF, 0x0F, 0x00, 0x68, 0x1F, 0x00, 0x09,
     0x00, 0xC6, 0x05, -1, -1, -1, -1, 0x01});

static Cave(tick_hook, (void*)&before_tick, (void*)&after_tick);

void init_tick_hook(){
    static uintptr_t sig_addr = sig_tick.get_address();
    tick_hook.build(sig_addr, 7);
    tick_hook.apply();
}

void revert_tick_hook(){
    tick_hook.revert();
}
