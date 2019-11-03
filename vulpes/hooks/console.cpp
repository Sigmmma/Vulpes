/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include "../../hooker/hooker.hpp"
#include "../functions/messaging.hpp"
#include "../command/handler.hpp"

Signature(true, sig_console_input_hook,
    {0x3C, 0x23, 0x74, 0x0A, 0x3C, 0x2F, 0x75, 0x0F});

extern "C" {
    uintptr_t console_hook__return_to_halo_con_in;
    extern console_hook__console_in();
}

Patch(console_in_hook_patch, sig_console_input_hook, 0, 6, JMP_PATCH, &console_hook__console_in);

void init_console_input_hook(){
    if(console_in_hook_patch.build()){
        console_hook__return_to_halo_con_in = console_in_hook_patch.address() + 23;
        console_in_hook_patch.apply();
    };
}

void revert_console_input_hook(){
    console_in_hook_patch.revert();
}

Signature(false, sig_auto_complete_hook,
    {0x0F, 0xBF, 0x15, -1, -1, -1, -1, 0x68, -1, -1, -1, -1, 0x6A, 0x04, 0x52, 0x55, 0xE8});
Signature(false, sig_auto_complete_collected_list,
    {-1, -1, -1, -1, 0x0F, 0xBF, 0xC8, 0x66, 0x40, 0x89, 0x34, 0x8A, 0x66, 0xA3,
     -1, -1, -1, -1, 0x83, 0xC3, 0x04, 0x4D, 0x75, 0x98, 0x5F});

extern "C" {
    intptr_t console_hook__results_ptr;
    intptr_t console_hook__count_ptr;
    extern console_hook__auto_complete();
}

Patch(auto_complete_patch, sig_auto_complete_hook, 0, 7, CALL_PATCH, &console_hook__auto_complete);

void init_command_auto_complete_hook(){
    static uintptr_t sig_addr3 = sig_auto_complete_collected_list.address();

    if (auto_complete_patch.build() && sig_addr3){
        console_hook__results_ptr = *reinterpret_cast<intptr_t*>(sig_addr3);
        console_hook__count_ptr   = *reinterpret_cast<intptr_t*>(sig_addr3+14);
        auto_complete_patch.apply();
    }else{
        cprintf_error("Error: Couldn't perform auto complete patch. "
                      "Vulpes commands will not auto complete.");
    };
}

void revert_command_auto_complete_hook(){
    auto_complete_patch.revert();
}

// Potential command suggestion hook location 0x44CD20
// Block autocomplete print for command suggestion hook 0x4CA00F
// v_sv_execute_file 0x4C9820
//
// execute from rcon
// 6A 00 A3 ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 04 C7 05 ?? ?? ?? ?? FF FF FF FF C3

void init_console_hooks(){
    init_console_input_hook();
    init_command_auto_complete_hook();
}

void revert_console_hooks(){
    revert_console_input_hook();
    revert_command_auto_complete_hook();
}
