/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include "../../hooker/hooker.hpp"
#include "../functions/messaging.hpp"
#include "../../command/handler.hpp"

Signature(true, sig_console_input_hook,
    {0x3C, 0x23, 0x74, 0x0A, 0x3C, 0x2F, 0x75, 0x0F});

const auto prcs_cmd = &process_command;
uintptr_t return_to_halo_con_in;
__attribute__((naked))
void new_console_in_hook(){
    asm (
    "console_in:"
        "push edi;"
        "call %0;"
        "pop edi;"

        "cmp al, 0;"
        "jne continue_to_halo_con_in;"
        "xor al, al;"
        "add esp, 0x500;"
        "ret;"
    "continue_to_halo_con_in:"
        "jmp %1;"
        :
        : "m" (prcs_cmd), "m" (return_to_halo_con_in)
    );
}

Patch(console_in_hook_patch, sig_console_input_hook, 0, 6, JMP_PATCH, &new_console_in_hook);

void init_console_input_hook(){
    if(console_in_hook_patch.build()){
        return_to_halo_con_in = console_in_hook_patch.address() + 23;
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

static auto func_auto_complete = &auto_complete;
static intptr_t results_ptr;
static intptr_t count_ptr;

__attribute__((naked))
void auto_complete_hook(){
    asm (
        // Get the address of the list of matching char*s
        "mov edx, %1;"
        "mov edx, [edx];"
        // Shove all of this data into our autocomplete function
        "push %2;"
        "push edx;"
        "call %0;"
        "add esp, 8;"
        // move the output count to where the code expects it to be
        "mov edx, %2;"
        "movsx edx, WORD PTR ds:[edx];"
        "ret;"
        :
        : "m" (func_auto_complete), "m" (results_ptr), "m" (count_ptr)
    );
}

Patch(auto_complete_patch, sig_auto_complete_hook, 0, 7, CALL_PATCH, &auto_complete_hook);

void init_command_auto_complete_hook(){
    static uintptr_t sig_addr3 = sig_auto_complete_collected_list.address();

    if (auto_complete_patch.build() && sig_addr3){
        results_ptr       = *reinterpret_cast<intptr_t*>(sig_addr3);
        count_ptr         = *reinterpret_cast<intptr_t*>(sig_addr3+14);
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
