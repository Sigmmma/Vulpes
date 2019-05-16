#include "framerate_dependent_timers.hpp"
#include "../../hooker/hooker.hpp"
#include "../hooks/tick.hpp"

Signature(false, sig_death_timer_framerate_dep,
    {0x38, 0x1D, -1, -1, -1, -1, 0x74, 0x33, 0xA1, -1, -1, -1, -1, 0x38, 0x58,
     0x02, 0x75, 0x29, 0x66, 0xA1, -1, -1, -1, -1, 0x66, 0x8B, 0xC8, 0x66, 0x40});

PatchNew(death_timer_framerate_dep_fix, 0, 2, NOP_PATCH, 0);

bool*    player_dead;
int32_t* player_respawn_timer;

void increment_respawn_timer(){
    if (*player_dead){
        *player_respawn_timer = *player_respawn_timer + 1;
    };
}

static bool initialized = false;

void init_checkpoint_revert_fix(){
    static intptr_t sig_addr = sig_death_timer_framerate_dep.address();
    if (!initialized && sig_addr){
        death_timer_framerate_dep_fix.build(sig_addr+27);
        player_dead = *(bool**)(sig_addr+2);
        player_respawn_timer = *(int32_t**)(sig_addr+20);
        initialized = true;
    };
    if (initialized){
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

PatchNew(patch_scoreboard_framerate_dep1a, sig_scoreboard_framerate_dep,   2, 4, INT_PATCH, &fade);
PatchNew(patch_scoreboard_framerate_dep1b, sig_scoreboard_framerate_dep,  10, 4, INT_PATCH, &fade);
PatchNew(patch_scoreboard_framerate_dep2a, sig_scoreboard_framerate_dep2,  2, 4, INT_PATCH, &fade);
PatchNew(patch_scoreboard_framerate_dep3a, sig_scoreboard_framerate_dep3,  2, 4, INT_PATCH, &fade);
PatchNew(patch_scoreboard_framerate_dep3b, sig_scoreboard_framerate_dep3, 14, 4, INT_PATCH, &fade);
PatchNew(patch_ruleboard_intro_nop, sig_scoreboard_ruleboard_intro_nop,    6, 4, INT_PATCH, 0);

void init_scoreboard_fix(){
    if (patch_scoreboard_framerate_dep1a.build()
    &&  patch_scoreboard_framerate_dep1b.build()
    &&  patch_scoreboard_framerate_dep2a.build()
    &&  patch_scoreboard_framerate_dep3a.build()
    &&  patch_scoreboard_framerate_dep3b.build()){
        patch_scoreboard_framerate_dep1a.apply();
        patch_scoreboard_framerate_dep1b.apply();
        patch_scoreboard_framerate_dep2a.apply();
        patch_scoreboard_framerate_dep3a.apply();
        patch_scoreboard_framerate_dep3b.apply();
    };
    if (patch_ruleboard_intro_nop.build()) patch_ruleboard_intro_nop.apply();
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

PatchNew(patch_console_framerate_dep, sig_console_framerate_dep, 6, 5, NOP_PATCH, 0);

void fade_console(){
    if (!*console_open){
        fade_console_halo();
    };
}

void init_console_fix(){
    static intptr_t sig_addr1 = sig_console_fade_call.address();
    if (sig_addr1 && patch_console_framerate_dep.build()){
        fade_console_halo = reinterpret_cast<void (*)()>(sig_addr1);
        console_open = *(bool**)(patch_console_framerate_dep.address()-6);
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
