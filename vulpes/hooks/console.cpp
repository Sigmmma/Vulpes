/*
 * This file is part of Vulpes, an extension of Halo Custom Edition's capabilities.
 * Copyright (C) 2019-2020 gbMichelle (Michelle van der Graaf)
 *
 * Vulpes is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, version 3.
 *
 * Vulpes is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * long with Vulpes.  If not, see <https://www.gnu.org/licenses/agpl-3.0.en.html>
 */

#include <hooker/hooker.hpp>

#include <vulpes/command/handler.hpp>
#include <vulpes/functions/messaging.hpp>
#include <vulpes/memory/signatures/signatures.hpp>

extern "C" {
    uintptr_t console_hook__return_to_halo_con_in;
    extern console_hook__console_in();
}

static Patch(console_in_hook_patch,
    NULL, 6, JMP_PATCH, &console_hook__console_in);

void init_console_input_hook() {
    auto addr = sig_hook_console_input();
    // Jump to this code after our console hook has been executed.
    // This skips the original code that checks if the line is a comment.
    // We can do this because we've implemented this exact check into our hook.
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

static Patch(auto_complete_patch, NULL, 7,
    CALL_PATCH, &console_hook__auto_complete);

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
