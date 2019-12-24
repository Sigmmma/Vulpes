/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <hooker/hooker.hpp>

#include <vulpes/hooks/tick.hpp>
#include <vulpes/memory/signatures.hpp>

#include "framerate_dependent_timers.hpp"

static Patch(death_timer_framerate_dep_fix, 0, 2, NOP_PATCH, 0);

bool*    player_dead;
int32_t* player_respawn_timer;

void increment_respawn_timer() {
    if (*player_dead) {
        *player_respawn_timer = *player_respawn_timer + 1;
    }
}

static bool initialized = false;

void init_checkpoint_revert_fix() {
    static intptr_t sig_addr = sig_fix_death_timer_framerate_dep();
    if (!initialized && sig_addr) {
        death_timer_framerate_dep_fix.build(sig_addr+27);
        player_dead = *reinterpret_cast<bool**>(sig_addr+2);
        player_respawn_timer = *reinterpret_cast<int32_t**>(sig_addr+20);
        initialized = true;
    }
    if (initialized) {
        death_timer_framerate_dep_fix.apply();
        ADD_CALLBACK(EVENT_TICK, increment_respawn_timer);
    }
}

void revert_checkpoint_revert_fix() {
    death_timer_framerate_dep_fix.revert();
    DEL_CALLBACK(EVENT_TICK, increment_respawn_timer);
}

// In assembly floats are referenced by pointers.
// This means we need a space to hold the value that the code will be using.
// Aka, this float.
static const float fade = 1.0;

static Patch(patch_scoreboard_framerate_dep1a, 0, 4, INT_PATCH, &fade);
static Patch(patch_scoreboard_framerate_dep1b, 0, 4, INT_PATCH, &fade);
static Patch(patch_scoreboard_framerate_dep2a, 0, 4, INT_PATCH, &fade);
static Patch(patch_scoreboard_framerate_dep3a, 0, 4, INT_PATCH, &fade);
static Patch(patch_scoreboard_framerate_dep3b, 0, 4, INT_PATCH, &fade);
static Patch(patch_ruleboard_intro_nop, 0, 4, INT_PATCH, 0);

void init_scoreboard_fix() {
    auto p_addr1 = sig_fix_scoreboard_framerate_dep1();
    auto p_addr2 = sig_fix_scoreboard_framerate_dep2();
    auto p_addr3 = sig_fix_scoreboard_framerate_dep3();
    if (!(p_addr1 && p_addr2 && p_addr3)) {
        return;
    }
    if (patch_scoreboard_framerate_dep1a.build(p_addr1+2)
    &&  patch_scoreboard_framerate_dep1b.build(p_addr1+10)
    &&  patch_scoreboard_framerate_dep2a.build(p_addr2+2)
    &&  patch_scoreboard_framerate_dep3a.build(p_addr3+2)
    &&  patch_scoreboard_framerate_dep3b.build(p_addr3+14)) {
        patch_scoreboard_framerate_dep1a.apply();
        patch_scoreboard_framerate_dep1b.apply();
        patch_scoreboard_framerate_dep2a.apply();
        patch_scoreboard_framerate_dep3a.apply();
        patch_scoreboard_framerate_dep3b.apply();
    }
    if (sig_fix_scoreboard_ruleboard_intro_nop()) {
        if (patch_ruleboard_intro_nop.build(
                sig_fix_scoreboard_ruleboard_intro_nop()+6))
            patch_ruleboard_intro_nop.apply();
    }

}

void revert_scoreboard_fix() {
    patch_scoreboard_framerate_dep1a.revert();
    patch_scoreboard_framerate_dep1b.revert();
    patch_scoreboard_framerate_dep2a.revert();
    patch_scoreboard_framerate_dep3a.revert();
    patch_scoreboard_framerate_dep3b.revert();
    patch_ruleboard_intro_nop.revert();
}

static bool console_initialized = false;

static bool* console_open;
static void (*fade_console_halo)();

static Patch(patch_console_framerate_dep, 0, 5, NOP_PATCH, 0);

void fade_console() {
    if (!*console_open) {
        fade_console_halo();
    }
}

void init_console_fix() {
    auto sig_addr1 = sig_fix_console_fade_call();
    auto sig_addr2 = sig_fix_console_framerate_dep();
    if (sig_addr1 && patch_console_framerate_dep.build(sig_addr2)) {
        fade_console_halo = reinterpret_cast<void (*)()>(sig_addr1);
        console_open = *reinterpret_cast<bool**>(sig_addr2-6);
        patch_console_framerate_dep.apply();
        ADD_CALLBACK(EVENT_TICK, fade_console);
    }
}

void revert_console_fix() {
    patch_console_framerate_dep.revert();
    DEL_CALLBACK(EVENT_TICK, fade_console);
}

void init_framerate_dependent_timer_fixes() {
    init_checkpoint_revert_fix();
    init_scoreboard_fix();
    init_console_fix();
}

void revert_framerate_dependent_timer_fixes() {
    revert_checkpoint_revert_fix();
    revert_scoreboard_fix();
    revert_console_fix();
}
