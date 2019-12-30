/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <vulpes/memory/signatures.hpp>

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
