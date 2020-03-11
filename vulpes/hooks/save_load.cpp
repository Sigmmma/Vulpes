/*
 * Vulpes (c) 2020 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <windows.h>

#include <hooker/hooker.hpp>
#include <hooker/function_pointer_safe.hpp>

#include <vulpes/functions/messaging.hpp>
#include <vulpes/memory/signatures.hpp>

#include "save_load.hpp"

static bool doing_core = false;

static void (*before_save_proc_orig)();
static void (*before_load_proc_orig)();
static void (*after_load_proc_orig)();
static int (*core_load_orig)(char*);

static void before_save_proc_hook() {
    cprintf_info("Before save hook ping!");
    exec_if_valid(before_save_proc_orig);
}

static void before_load_proc_hook() {
    if (!doing_core) {
        cprintf_info("Before load hook ping!");
    }
    exec_if_valid(before_load_proc_orig);
}

static void after_load_proc_hook() {
    exec_if_valid(after_load_proc_orig);
    if (!doing_core) {
        cprintf_info("After load hook ping!");
    }
}

// https://github.com/Sigmmma/Vulpes/issues/68
// This function was a wrapper to avoid calling checkpoint load logic during
// a core_load. Because there is distinction.
// But core loading and saving is kind of a hack.
__attribute__((regparm(1)))
static int core_load_hook(char* name) {
    cprintf_error("core_load is disabled");
    //doing_core = true;
    //int result = core_load_orig(name);
    //doing_core = false;
    //cprintf_info("After core_load ping!");
    return 0;//result;
}

static Patch(
    core_load_hook_patch, NULL, 5,
    CALL_PATCH, &core_load_hook
);

static bool hook_save_load_initialized = false;

void init_save_load_hook() {
    if (!hook_save_load_initialized) {
        hook_save_load_initialized = true;
        auto function_pointers = *sig_hook_save_load();

        typedef void (*fp_type)();
        // All of these are stored in arrays that in both 1.10 client and
        // server (our only targets) are right next to each other.
        before_save_proc_orig = reinterpret_cast<fp_type>(function_pointers[0]);
        before_load_proc_orig = reinterpret_cast<fp_type>(function_pointers[1]);
        after_load_proc_orig  = reinterpret_cast<fp_type>(function_pointers[14]);

        auto area = reinterpret_cast<size_t>(function_pointers[15]) - reinterpret_cast<size_t>(function_pointers[0]);

        DWORD prota, protb;
        VirtualProtect(&function_pointers[0], area, PAGE_EXECUTE_READWRITE, &prota);

        function_pointers[0]  = reinterpret_cast<uintptr_t>(&before_save_proc_hook);
        function_pointers[1]  = reinterpret_cast<uintptr_t>(&before_load_proc_hook);
        function_pointers[14] = reinterpret_cast<uintptr_t>(&after_load_proc_hook);

        VirtualProtect(&function_pointers[0], area, prota, &protb);

        auto core_load_fix_hack_addr = sig_hook_core_load();
        if (core_load_fix_hack_addr) {
            core_load_orig = reinterpret_cast<int(*)(char*)>(get_call_address(core_load_fix_hack_addr));
            core_load_hook_patch.build(core_load_fix_hack_addr);
            core_load_hook_patch.apply();
        }
    }
}

void revert_save_load_hook() {
    if (hook_save_load_initialized) {
        hook_save_load_initialized = false;
        auto function_pointers = *sig_hook_save_load();

        auto area = reinterpret_cast<size_t>(function_pointers[15]) - reinterpret_cast<size_t>(function_pointers[0]);

        DWORD prota, protb;
        VirtualProtect(&function_pointers[0], area, PAGE_EXECUTE_READWRITE, &prota);

        function_pointers[0]  = reinterpret_cast<uintptr_t>(&before_save_proc_orig);
        function_pointers[1]  = reinterpret_cast<uintptr_t>(&before_load_proc_orig);
        function_pointers[14] = reinterpret_cast<uintptr_t>(&after_load_proc_orig);

        VirtualProtect(&function_pointers[0], area, prota, &protb);

        core_load_hook_patch.revert();
    }
}
