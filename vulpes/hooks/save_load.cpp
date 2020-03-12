/*
 * Vulpes (c) 2020 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <windows.h>

#include <hooker/hooker.hpp>
#include <hooker/function_pointer_safe.hpp>

#include <vulpes/memory/signatures.hpp>

#include "save_load.hpp"

DEFINE_EVENT_HOOK_LIST(EVENT_BEFORE_SAVE, before_save_events);
DEFINE_EVENT_HOOK_LIST(EVENT_BEFORE_LOAD, before_load_events);
DEFINE_EVENT_HOOK_LIST(EVENT_AFTER_LOAD,  after_load_events);

static bool doing_core = false;

typedef void (*fp_type)();
typedef __attribute__((regparm(1))) int (*core_load_type)(char*);
static fp_type before_save_proc_orig;
static fp_type before_load_proc_orig;
static fp_type after_load_proc_orig;
static core_load_type core_load_orig;

static void before_save_proc_hook() {
    call_in_order(before_save_events);
    exec_if_valid(before_save_proc_orig);
}

static void before_load_proc_hook() {
    if (!doing_core) {
        call_in_order(before_load_events);
    }
    exec_if_valid(before_load_proc_orig);
    // When this function finishes executing the vanilla gamestate is loaded
    // from disk/in-memory copy.
}

static void after_load_proc_hook() {
    exec_if_valid(after_load_proc_orig);
    if (!doing_core) {
        call_in_order(after_load_events);
    }
}

// https://github.com/Sigmmma/Vulpes/issues/68
// This function was a wrapper to avoid calling checkpoint load logic during
// a core_load. Because there is distinction.
// But core loading and saving is kind of a hack.
__attribute__((regparm(1)))
static int core_load_hook(char* name) {
    doing_core = true;
    int result = core_load_orig(name);
    doing_core = false;
    return result;
}

static Patch(
    core_load_hook_patch, NULL, 5,
    CALL_PATCH, &core_load_hook
);

static bool hook_save_load_initialized = false;

void init_save_load_hook() {
    if (!hook_save_load_initialized) {
        hook_save_load_initialized = true;

        auto function_pointers = reinterpret_cast<fp_type*>(*sig_hook_save_load());
        // All of these are stored in arrays that in both 1.10 client and
        // server (our only targets) are right next to each other.
        before_save_proc_orig = function_pointers[0];
        before_load_proc_orig = function_pointers[1];
        after_load_proc_orig  = function_pointers[14];

        auto area = reinterpret_cast<size_t>(function_pointers[15]) - reinterpret_cast<size_t>(function_pointers[0]);

        DWORD prota, protb;
        VirtualProtect(&function_pointers[0], area, PAGE_EXECUTE_READWRITE, &prota);

        function_pointers[0]  = &before_save_proc_hook;
        function_pointers[1]  = &before_load_proc_hook;
        function_pointers[14] = &after_load_proc_hook;

        VirtualProtect(&function_pointers[0], area, prota, &protb);

        auto core_load_fix_hack_addr = sig_hook_core_load();
        if (core_load_fix_hack_addr) {
            core_load_orig = reinterpret_cast<core_load_type>(get_call_address(core_load_fix_hack_addr));
            core_load_hook_patch.build(core_load_fix_hack_addr);
            core_load_hook_patch.apply();
        }
    }
}

void revert_save_load_hook() {
    if (hook_save_load_initialized) {
        hook_save_load_initialized = false;
        auto function_pointers = reinterpret_cast<fp_type*>(*sig_hook_save_load());

        auto area = reinterpret_cast<size_t>(function_pointers[15]) - reinterpret_cast<size_t>(function_pointers[0]);

        DWORD prota, protb;
        VirtualProtect(&function_pointers[0], area, PAGE_EXECUTE_READWRITE, &prota);

        function_pointers[0]  = before_save_proc_orig;
        function_pointers[1]  = before_load_proc_orig;
        function_pointers[14] = after_load_proc_orig;

        VirtualProtect(&function_pointers[0], area, prota, &protb);

        core_load_hook_patch.revert();
    }
}
