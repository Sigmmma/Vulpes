#include "hooker.hpp"
#include <cstdint>
#include <string.h>
#include "../halo_functions/console.hpp"
#include "../command/handler.hpp"


Signature(false, sig_auto_complete_hook,
    {0x0F, 0xBF, 0x15, -1, -1, -1, -1, 0x68, -1, -1, -1, -1, 0x6A, 0x04, 0x52, 0x55, 0xE8});
Signature(false, sig_auto_complete_console_input,
    {-1, -1, -1, -1, 0x8B, 0xC2, 0x8D, 0x78, 0x01, 0xEB, 0x03, 0x8D, 0x49, 0x00});
Signature(false, sig_auto_complete_collected_list,
    {-1, -1, -1, -1, 0x0F, 0xBF, 0xC8, 0x66, 0x40, 0x89, 0x34, 0x8A, 0x66, 0xA3,
     -1, -1, -1, -1, 0x83, 0xC3, 0x04, 0x4D, 0x75, 0x98, 0x5F});


Patch(auto_complete_patch);

intptr_t func_auto_complete = (intptr_t)&auto_complete;
intptr_t console_input_ptr;
intptr_t results_ptr;
intptr_t count_ptr;

__attribute__((naked))
void auto_complete_hook(){
    asm (
        // Get the address at which the console input text is
        "mov ebx, %1;\n"
        "mov ebx, [ebx];\n"
        // Get the address of the list of matching char*s
        "mov edx, %2;\n"
        "mov edx, [edx];\n"
        // Get the number of suggestions that we already have.
        "mov eax, %3;\n"
        "mov ax, [eax];\n"
        // Shove all of this data into our autocomplete function
        "push ebx;\n"
        "push ax;\n"
        "push edx;\n"
        "call %0;\n"
        "add esp, 10;\n"
        // move the output count into the place where it belongs
        "mov edx, %3;\n"
        "mov WORD PTR ds:[edx], ax;\n"
        // move the output count to where the code expects it to be
        "movsx edx, ax;\n"
        "ret;\n"
        : "+m" (func_auto_complete)
        : "m" (console_input_ptr),
          "m" (results_ptr),
          "m" (count_ptr)
    );
}

void init_command_auto_complete_hook(){
    uintptr_t sig_addr = sig_auto_complete_hook.get_address();
    uintptr_t sig_addr2 = sig_auto_complete_console_input.get_address();
    uintptr_t sig_addr3 = sig_auto_complete_collected_list.get_address();

    if (sig_addr && sig_addr2 && sig_addr3){
        auto_complete_patch.build(sig_addr, 7, CALL_PATCH, (uintptr_t)&auto_complete_hook);
        console_input_ptr = *(intptr_t*)sig_addr2;
        results_ptr       = *(intptr_t*)sig_addr3;
        count_ptr         = *(intptr_t*)(sig_addr3+14);

    };
    if (auto_complete_patch.is_built()){
        auto_complete_patch.apply();
    }else{
        console_out_error("Error: Couldn't add auto complete patch. Vulpes commands will not auto complete.");
    };
}

void revert_command_auto_complete_hook(){
    auto_complete_patch.revert();
}

void init_console_hooks(){
    //init_command_in_hook();
    init_command_auto_complete_hook();
}

void revert_console_hooks(){
    //revert_command_in_hook();
    revert_command_auto_complete_hook();
}
/*
tab completion flags 0x14
A3????????750AC705

Tab Completion 0 -- place our call here
0FBF15????????68????????6A045255E8
*/
