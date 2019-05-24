#include "console.hpp"
#include "../../../hooker/hooker.hpp"

Signature(true, sig_console_out2,
    {0x83, 0xEC, 0x10, 0x57, 0x8B, 0xF8, 0xA0, -1, -1, -1, -1,
     0x84, 0xC0, 0xC7, 0x44, 0x24, 0x04, 0x00, 0x00, 0x80, 0x3F});

ConsoleGlobals* console_globals(){
    static ConsoleGlobals* console_glob_ptr =
        *reinterpret_cast<ConsoleGlobals**>(sig_console_out2.address()+7);
    return console_glob_ptr;
}

ConsoleOutputTable* console_output_table(){
    return console_globals()->output;
}
// .address() -> address from spot -> address at address -> table
// *             *                    *
// We save the ** because then if the pointer gets changed we don't need to
// reinitialize. We return * so it's easier to code with.
