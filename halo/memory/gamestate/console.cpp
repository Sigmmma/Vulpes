/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include "console.hpp"
#include "../../../hooker/hooker.hpp"

Signature(true, sig_console_out2,
    {0x83, 0xEC, 0x10, 0x57, 0x8B, 0xF8, 0xA0, -1, -1, -1, -1,
     0x84, 0xC0, 0xC7, 0x44, 0x24, 0x04, 0x00, 0x00, 0x80, 0x3F});
Signature(true, sig_console_input_globals,
    {0x38, 0x1D, -1, -1, -1, -1, 0x0F, 0x85, -1, -1, 0x00, 0x00,
     0xA1, -1, -1, -1, -1, 0x38, 0x18, 0x74});

ConsoleGlobals* console_globals(){
    static ConsoleGlobals* console_glob_ptr =
        *reinterpret_cast<ConsoleGlobals**>(sig_console_out2.address()+7);
    return console_glob_ptr;
}

ConsoleOutputTable* console_output_table(){
    return console_globals()->output;
}

ConsoleInputGlobals* console_input_globals(){
    static ConsoleInputGlobals* console_input_glob_ptr =
        *reinterpret_cast<ConsoleInputGlobals**>(sig_console_input_globals.address()+2);
    return console_input_glob_ptr;
}
// .address() -> address from spot -> address at address -> table
// *             *                    *
// We save the ** because then if the pointer gets changed we don't need to
// reinitialize. We return * so it's easier to code with.
