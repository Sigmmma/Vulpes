#include "framerate_dependent_timers.hpp"
#include "../../hooker/hooker.hpp"
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

void init_checkpoint_revert_fix(){
    intptr_t sig_addr = sig_death_timer_framerate_dep.get_address();
    if (sig_addr && !death_timer_framerate_dep_fix.is_built()){
        death_timer_framerate_dep_fix.build_old(sig_addr+27, 2, NOP_PATCH, 0);
        player_dead = *(bool**)(sig_addr+2);
        player_respawn_timer = *(int32_t**)(sig_addr+20);
    };
    if (death_timer_framerate_dep_fix.is_built()){
        death_timer_framerate_dep_fix.apply();
        ADD_CALLBACK(EVENT_TICK, increment_respawn_timer);
    };
}

void revert_checkpoint_revert_fix(){
    death_timer_framerate_dep_fix.revert();
    DEL_CALLBACK(EVENT_TICK, increment_respawn_timer);
}

Signature(false, sig_scoreboard_framerate_dep,
    {0xD8, 0x25, -1, -1, -1, -1, 0xEB, 0x06, 0xD8, 0x05});

Signature(false, sig_scoreboard_framerate_dep2,
    {0xD8, 0x05, -1, -1, -1, -1, 0xEB, 0x42});

Signature(false, sig_scoreboard_framerate_dep3,
    {0xD8, 0x05, -1, -1, -1, -1, 0xEB, 0x0A, 0xD9, 0x44, 0x24, 0x14, 0xD8, 0x25});

Signature(false, sig_scoreboard_ruleboard_intro_nop,
    {0xC7, 0x05, -1, -1, -1, 0x00, 0x96, 0x00, 0x00, 0x00, 0x5B});

const float fade = 1.0;

static bool scoreboard_initialized = false;

Patch(patch_scoreboard_framerate_dep1a);
Patch(patch_scoreboard_framerate_dep1b);
Patch(patch_scoreboard_framerate_dep2a);
Patch(patch_scoreboard_framerate_dep3a);
Patch(patch_scoreboard_framerate_dep3b);
Patch(patch_ruleboard_intro_nop);

void init_scoreboard_fix(){
    static intptr_t sig_addr1 = sig_scoreboard_framerate_dep.get_address();
    static intptr_t sig_addr2 = sig_scoreboard_framerate_dep2.get_address();
    static intptr_t sig_addr3 = sig_scoreboard_framerate_dep3.get_address();
    static intptr_t sig_addr4 = sig_scoreboard_ruleboard_intro_nop.get_address();
    if (!scoreboard_initialized && sig_addr1 && sig_addr2 && sig_addr3 && sig_addr4){
        patch_scoreboard_framerate_dep1a.build_int(sig_addr1+2, (int32_t)&fade);
        patch_scoreboard_framerate_dep1b.build_int(sig_addr1+10,(int32_t)&fade);
        patch_scoreboard_framerate_dep2a.build_int(sig_addr2+2, (int32_t)&fade);
        patch_scoreboard_framerate_dep3a.build_int(sig_addr3+2, (int32_t)&fade);
        patch_scoreboard_framerate_dep3b.build_int(sig_addr3+14,(int32_t)&fade);
        patch_ruleboard_intro_nop.build_int(sig_addr4+6, 0);

        scoreboard_initialized = true;
    };
    if (scoreboard_initialized){
        patch_scoreboard_framerate_dep1a.apply();
        patch_scoreboard_framerate_dep1b.apply();
        patch_scoreboard_framerate_dep2a.apply();
        patch_scoreboard_framerate_dep3a.apply();
        patch_scoreboard_framerate_dep3b.apply();
        patch_ruleboard_intro_nop.apply();
    };
}

void revert_scoreboard_fix(){
    patch_scoreboard_framerate_dep1a.revert();
    patch_scoreboard_framerate_dep1b.revert();
    patch_scoreboard_framerate_dep2a.revert();
    patch_scoreboard_framerate_dep3a.revert();
    patch_scoreboard_framerate_dep3b.revert();
    patch_ruleboard_intro_nop.revert();
}

Signature(false, sig_console_fade_call,
    {0x8B, 0x15, -1, -1, -1, -1, 0x83, 0xFA, 0xFF, 0x74});
Signature(false, sig_console_framerate_dep,
    {-1, -1, -1, 0x00, 0x75, 0x05, 0xE8, -1, -1, -1, 0xFF,
     0xe8, -1, -1, -1, 0xFF});

static bool console_initialized = false;

static bool* console_open;
static void (*fade_console_halo)();

Patch(patch_console_framerate_dep);

void fade_console(){
    if (!*console_open){
        fade_console_halo();
    };
}

void init_console_fix(){
    static intptr_t sig_addr1 = sig_console_fade_call.get_address();
    static intptr_t sig_addr2 = sig_console_framerate_dep.get_address();
    if (!console_initialized && sig_addr1 && sig_addr2){
        fade_console_halo = reinterpret_cast<void (*)()>(sig_addr1);
        console_open = (bool*)*(uintptr_t*)sig_addr2;
        patch_console_framerate_dep.build_old(sig_addr2+6, 5, NOP_PATCH, 0);
        console_initialized = true;
    };
    if (console_initialized){
        patch_console_framerate_dep.apply();
        ADD_CALLBACK(EVENT_TICK, fade_console);
    };
}

void revert_console_fix(){
    patch_console_framerate_dep.revert();
    DEL_CALLBACK(EVENT_TICK, fade_console);
}

void init_framerate_dependent_timer_fixes(){
    init_checkpoint_revert_fix();
    init_scoreboard_fix();
    init_console_fix();
}

void revert_framerate_dependent_timer_fixes(){
    revert_checkpoint_revert_fix();
    revert_scoreboard_fix();
    revert_console_fix();
}
