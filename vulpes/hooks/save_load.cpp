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
    // Execute the original function.
    exec_if_valid(before_save_proc_orig);
}

static void before_load_proc_hook() {
    if (!doing_core) {
        call_in_order(before_load_events);
    }
    // Execute the original function.
    exec_if_valid(before_load_proc_orig);
    // When this function finishes executing the vanilla gamestate is loaded
    // from disk/in-memory copy.
}

static void after_load_proc_hook() {
    // Execute the original function.
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

static const size_t BEFORE_SAVE_FUNC_OVERWRITE_ID = 0; // The only before_save function pointer.
static const size_t BEFORE_LOAD_FUNC_OVERWRITE_ID = 1; // The only before_load function pointer.
static const size_t AFTER_LOAD_FUNC_OVERWRITE_ID = 14; // The last after_load function pointer.

void init_save_load_hook() {
    if (!hook_save_load_initialized) {
        hook_save_load_initialized = true;

        auto function_pointers = reinterpret_cast<fp_type*>(*sig_hook_save_load());
        // All of these are stored in arrays that in both 1.10 client and
        // server (our only targets) are right next to each other.
        before_save_proc_orig = function_pointers[BEFORE_SAVE_FUNC_OVERWRITE_ID];
        before_load_proc_orig = function_pointers[BEFORE_LOAD_FUNC_OVERWRITE_ID];
        after_load_proc_orig  = function_pointers[AFTER_LOAD_FUNC_OVERWRITE_ID];

        // The area of our protection change should include all the function pointers.
        // Including the bytes of element 14.
        auto protection_range =
            reinterpret_cast<size_t>(&function_pointers[AFTER_LOAD_FUNC_OVERWRITE_ID+1])
            - reinterpret_cast<size_t>(&function_pointers[BEFORE_SAVE_FUNC_OVERWRITE_ID]);

        DWORD prota, protb;
        VirtualProtect(&function_pointers[BEFORE_SAVE_FUNC_OVERWRITE_ID],
            protection_range, PAGE_EXECUTE_READWRITE, &prota);

        function_pointers[BEFORE_SAVE_FUNC_OVERWRITE_ID] = &before_save_proc_hook;
        function_pointers[BEFORE_LOAD_FUNC_OVERWRITE_ID] = &before_load_proc_hook;
        function_pointers[AFTER_LOAD_FUNC_OVERWRITE_ID]  = &after_load_proc_hook;

        VirtualProtect(&function_pointers[BEFORE_SAVE_FUNC_OVERWRITE_ID],
            protection_range, prota, &protb);

        auto core_load_fix_hack_addr = sig_hook_core_load();
        if (core_load_fix_hack_addr) {
            core_load_orig = reinterpret_cast<core_load_type>(
                get_call_address(core_load_fix_hack_addr));
            core_load_hook_patch.build(core_load_fix_hack_addr);
            core_load_hook_patch.apply();
        }
    }
}

void revert_save_load_hook() {
    if (hook_save_load_initialized) {
        hook_save_load_initialized = false;
        auto function_pointers = reinterpret_cast<fp_type*>(*sig_hook_save_load());

        auto protection_range =
            reinterpret_cast<size_t>(&function_pointers[AFTER_LOAD_FUNC_OVERWRITE_ID+1])
            - reinterpret_cast<size_t>(&function_pointers[BEFORE_SAVE_FUNC_OVERWRITE_ID]);

        DWORD prota, protb;
        VirtualProtect(&function_pointers[BEFORE_SAVE_FUNC_OVERWRITE_ID],
            protection_range, PAGE_EXECUTE_READWRITE, &prota);

        function_pointers[BEFORE_SAVE_FUNC_OVERWRITE_ID] = before_save_proc_orig;
        function_pointers[BEFORE_LOAD_FUNC_OVERWRITE_ID] = before_load_proc_orig;
        function_pointers[AFTER_LOAD_FUNC_OVERWRITE_ID]  = after_load_proc_orig;

        VirtualProtect(&function_pointers[BEFORE_SAVE_FUNC_OVERWRITE_ID],
            protection_range, prota, &protb);

        core_load_hook_patch.revert();
    }
}
