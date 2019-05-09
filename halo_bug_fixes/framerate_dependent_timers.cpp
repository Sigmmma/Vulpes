#include "framerate_dependent_timers.hpp"
#include "../hooks/hooker.hpp"
#include "../hooks/tick.hpp"

Signature(false, sig_death_timer_framerate_dep,
    {0x38, 0x1D, -1, -1, -1, -1, 0x74, 0x33, 0xA1, -1, -1, -1, -1, 0x38, 0x58,
     0x02, 0x75, 0x29, 0x66, 0xA1, -1, -1, -1, -1, 0x66, 0x8B, 0xC8, 0x66, 0x40});

Patch(death_timer_framerate_dep_fix);

bool*    player_dead;
int32_t* player_respawn_timer;

void increment_respawn_timer(){
    if (*player_dead){
        *player_respawn_timer = *player_respawn_timer + 1;
    };
}

void init_framerate_dependent_timer_fixes(){
    intptr_t sig_addr = sig_death_timer_framerate_dep.get_address();
    if (sig_addr && !death_timer_framerate_dep_fix.is_built()){
        death_timer_framerate_dep_fix.build(sig_addr+6+2+5+3+2+6+3, 2, NOP_PATCH, 0);
        player_dead = (bool*)*(uintptr_t*)(sig_addr+2);
        player_respawn_timer = (int32_t*)*(uintptr_t*)(sig_addr+6+2+5+3+2+2);
    };
    if (death_timer_framerate_dep_fix.is_built()){
        death_timer_framerate_dep_fix.apply();
        ADD_EVENT(EVENT_TICK, increment_respawn_timer);
    };
}

void revert_framerate_dependent_timer_fixes(){
    death_timer_framerate_dep_fix.revert();
    DEL_EVENT(EVENT_TICK, increment_respawn_timer);
}
