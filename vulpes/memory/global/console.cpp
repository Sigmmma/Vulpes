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

#include <vulpes/memory/signatures/signatures.hpp>

#include "console.hpp"

ConsoleGlobals* console_globals() {
    ConsoleGlobals* console_glob_ptr =
        *reinterpret_cast<ConsoleGlobals**>(sig_console_globals());
    return console_glob_ptr;
}

ConsoleOutputTable* console_output_table() {
    return console_globals()->output;
}

ConsoleInputGlobals* console_input_globals() {
    ConsoleInputGlobals* console_input_glob_ptr =
        *reinterpret_cast<ConsoleInputGlobals**>(sig_console_input_globals());
    return console_input_glob_ptr;
}
// .address() -> address from spot -> address at address -> table
// *             *                    *
// We save the ** because then if the pointer gets changed we don't need to
// reinitialize. We return * so it's easier to code with.
