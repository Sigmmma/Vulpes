/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <hooker/hooker.hpp>

#include <vulpes/command/handler.hpp>
#include <vulpes/functions/messaging.hpp>
#include <vulpes/memory/signatures.hpp>

extern "C" {
    uintptr_t console_hook__return_to_halo_con_in;
    extern console_hook__console_in();
}

Patch(console_in_hook_patch, 0, 6, JMP_PATCH, &console_hook__console_in);

void init_console_input_hook() {
    auto addr = sig_hook_console_input();
    console_hook__return_to_halo_con_in = addr + 23;
    console_in_hook_patch.build(addr);
    console_in_hook_patch.apply();
}

void revert_console_input_hook() {
    console_in_hook_patch.revert();
}

extern "C" {
    intptr_t console_hook__results_ptr;
    intptr_t console_hook__count_ptr;
    extern console_hook__auto_complete();
}

Patch(auto_complete_patch, 0, 7, CALL_PATCH, &console_hook__auto_complete);

void init_command_auto_complete_hook() {
    auto adrr = sig_hook_auto_complete_collected_list();

    if (auto_complete_patch.build(sig_hook_auto_complete()) && adrr) {
        console_hook__results_ptr = *reinterpret_cast<intptr_t*>(adrr);
        console_hook__count_ptr   = *reinterpret_cast<intptr_t*>(adrr + 14);
        auto_complete_patch.apply();
    } else {
        cprintf_error("Error: Couldn't perform auto complete patch. "
                      "Vulpes commands will not auto complete.");
    }
}

void revert_command_auto_complete_hook() {
    auto_complete_patch.revert();
}

void init_console_hooks() {
    init_console_input_hook();
    init_command_auto_complete_hook();
}

void revert_console_hooks() {
    revert_console_input_hook();
    revert_command_auto_complete_hook();
}
