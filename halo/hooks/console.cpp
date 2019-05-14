#include "hooker.hpp"
#include "../halo_functions/console.hpp"
#include "../command/handler.hpp"

Signature(true, sig_console_input_hook,
    {0x3C, 0x23, 0x74, 0x0A, 0x3C, 0x2F, 0x75, 0x0F});
Patch(console_in_hook_patch);
Patch(rcon_in_hook_patch);

uintptr_t rcon_dword_ptr;
uintptr_t prcs_cmd = (uintptr_t)&process_command;
uintptr_t return_to_halo_con_in;
__attribute__((naked))
void new_console_in_hook(){
    asm (
        "add esp, 0x500;\n"
        "mov eax, 0xFFFFFFFF;\n" // No network owner
    "console_in:"
        "push eax;\n"
        "push edi;\n"
        "call %0;\n"
        "pop edi;\n"
        "add esp, 4;\n"

        "cmp al, 0;\n"
        "jne continue_to_halo_con_in;\n"
        "xor al, al;\n"
        "ret;\n"
    "continue_to_halo_con_in:\n"
        "sub esp, 0x500;\n"
        "jmp %1;\n"
        : "+m" (prcs_cmd)
        : "m" (return_to_halo_con_in)
    );
}

__attribute__((naked))
void rcon_in_hook(){
    asm (
        // Emulate default behavior of moving eax (player_network_id) into this dword.
        "push ebx;" // protect ebx
        "mov ebx, %0;\n"
        "mov DWORD PTR ds:[ebx], eax;\n"

        "push 0;\n"
        "call console_in;\n"
        "add esp, 4;\n"

        // Emulate default behavior of making this dword -1.
        "mov ebx, %0;\n"
        "mov DWORD PTR ds:[ebx], 0xFFFFFFFF;\n"

        "pop ebx;\n" // return ebx to its former glory
        "ret;\n"
        : "+m" (rcon_dword_ptr)
    );
}

void init_console_input_hook(){
    if(!console_in_hook_patch.is_built()){
        uintptr_t sig_addr = sig_console_input_hook.get_address();
        return_to_halo_con_in = sig_addr+0x1E + 5 - 12;
        console_in_hook_patch.build(sig_addr, 6, JMP_PATCH, (uintptr_t)&new_console_in_hook);
        rcon_dword_ptr = *(intptr_t*)(sig_addr - 32 + 3);
        rcon_in_hook_patch.build(sig_addr-32-12, 7+8+10, JMP_PATCH, (uintptr_t)&rcon_in_hook);
    };
    console_in_hook_patch.apply();
    rcon_in_hook_patch.apply();
}

void revert_console_input_hook(){
    console_in_hook_patch.revert();
    rcon_in_hook_patch.revert();
}

static Signature(false, sig_auto_complete_hook,
    {0x0F, 0xBF, 0x15, -1, -1, -1, -1, 0x68, -1, -1, -1, -1, 0x6A, 0x04, 0x52, 0x55, 0xE8});
static Signature(false, sig_auto_complete_collected_list,
    {-1, -1, -1, -1, 0x0F, 0xBF, 0xC8, 0x66, 0x40, 0x89, 0x34, 0x8A, 0x66, 0xA3,
     -1, -1, -1, -1, 0x83, 0xC3, 0x04, 0x4D, 0x75, 0x98, 0x5F});
static Signature(false, sig_console_input,
    {-1, -1, -1, -1, 0x8B, 0xC2, 0x8D, 0x78, 0x01, 0xEB, 0x03, 0x8D, 0x49, 0x00});
static Patch(auto_complete_patch);

static intptr_t func_auto_complete = (intptr_t)&auto_complete;
static intptr_t console_input_ptr;
static intptr_t results_ptr;
static intptr_t count_ptr;

__attribute__((naked))
void auto_complete_hook(){
    asm (
        // Get the address of the list of matching char*s
        "mov edx, %2;\n"
        "mov edx, [edx];\n"
        // Get the pointer to the console input text.
        "mov ebx, %1;\n"
        "mov ebx, [ebx];\n"
        // Shove all of this data into our autocomplete function
        "push ebx;\n"
        "push %3;\n"
        "push edx;\n"
        "call %0;\n"
        "add esp, 12;\n"
        // move the output count to where the code expects it to be
        "mov edx, %3;\n"
        "movsx edx, WORD PTR ds:[edx];\n"
        //"dec edx;\n"
        "ret;\n"
        : "+m" (func_auto_complete)
        : "m" (console_input_ptr),
          "m" (results_ptr),
          "m" (count_ptr)
    );
}

void init_command_auto_complete_hook(){
    uintptr_t sig_addr  = sig_auto_complete_hook.get_address();
    uintptr_t sig_addr2 = sig_console_input.get_address();
    uintptr_t sig_addr3 = sig_auto_complete_collected_list.get_address();

    if (sig_addr && sig_addr3){
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
