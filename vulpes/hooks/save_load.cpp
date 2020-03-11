/*
 * Vulpes (c) 2020 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <windows.h>
#include <cstdio>

#include <hooker/function_pointer_safe.hpp>

#include <vulpes/functions/messaging.hpp>
#include <vulpes/memory/signatures.hpp>

#include "save_load.hpp"

void (*before_save_proc_orig)();
void (*before_load_proc_orig)();
void (*after_load_proc_orig)();

void before_save_proc_hook() {
    cprintf_info("Before save hook ping!");
    exec_if_valid(before_save_proc_orig);
}

void before_load_proc_hook() {
    cprintf_info("Before load hook ping!");
    exec_if_valid(before_load_proc_orig);
}

void after_load_proc_hook() {
    exec_if_valid(after_load_proc_orig);
    cprintf_info("After load hook ping!");
}


static bool hook_save_load_initialized = false;

void init_save_load_hook() {
    if (!hook_save_load_initialized) {
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
    }
}

void revert_save_load_hook() {
    if (hook_save_load_initialized) {
        auto function_pointers = *sig_hook_save_load();

        auto area = reinterpret_cast<size_t>(function_pointers[15]) - reinterpret_cast<size_t>(function_pointers[0]);

        DWORD prota, protb;
        VirtualProtect(&function_pointers[0], area, PAGE_EXECUTE_READWRITE, &prota);

        function_pointers[0]  = reinterpret_cast<uintptr_t>(&before_save_proc_orig);
        function_pointers[1]  = reinterpret_cast<uintptr_t>(&before_load_proc_orig);
        function_pointers[14] = reinterpret_cast<uintptr_t>(&after_load_proc_orig);

        VirtualProtect(&function_pointers[0], area, prota, &protb);
    }
}
