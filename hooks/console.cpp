#include "hooker.hpp"
#include <cstdint>
#include <strings.h>
#include "../halo_functions/console.hpp"
#include "../command/handler.hpp"

// This area in the function is only executed when Halo doesn't recognize a command.
static Signature(false, sig_console_command_in_client,
    {0x66, 0x3B, 0xF3, 0x7E, 0x24, 0x8B, 0xFF, 0x0F, 0xBF, 0xCE});
static Signature(true, sig_console_command_in_server,
    {0x66, 0x3B, 0xF3, 0x7E, 0x26, 0x8D, 0x64, 0x24, 0x00, 0x0F, 0xBF, 0xCE});
static Signature(true, sig_console_command_in_not_found,
    {0x8D, 0x4C, 0x24, 0x08, 0x51, 0x68, -1, -1, -1, -1, 0x53});
static Signature(true, sig_console_input,
    {-1, -1, -1, -1, 0x8B, 0xC2, 0x8D, 0x78, 0x01, 0xEB, 0x03, 0x8D, 0x49, 0x00});
static Signature(true, sig_console_command_kavaworkaround,
    {0x57, 0xC6, 0x05, -1, -1, -1, -1, 0x01, 0xE8, -1, -1, -1, -1, 0x83, 0xC4, 0x04});

static Patch(command_in_patch);

bool cancel_default_routine = false;

__attribute__((cdecl))
void evaluate_command(const char** input){
    const char* input_str = *input;
    cancel_default_routine = false;
    printf("%s", input_str);
    // fooox
    if (strcasecmp("fox", input_str) == 0){
        printf("%s", "caught");
        console_out("EEEEEEEEP!");
        cancel_default_routine = true;
    };
}

static intptr_t func_evaluate_command = (intptr_t)&evaluate_command;
static intptr_t jmp_return_addr;
static intptr_t jmp_return_cancel;
static intptr_t jmp_return_continue;
static intptr_t console_input_ptr;
// We wouldn't need a jump to whatever the fuck this is if kava didn't have such
// an assinine cmd cancel
static intptr_t func_stricmp = (intptr_t)&strcasecmp;
static intptr_t whateverthefuck;

// Blame Kavawuvi's chimera for making this overly complicated

// TODO, rewrite most of this garbage in C++ so we don't have to look at it anymore
__attribute__((naked))
void console_in_hook(){
    asm (
        // Original code excluding the filler instruction
        "cmp si, bx;\n"
        "jle not_recognized_by_halo;\n"
    "loop:\n"
        "movsx   ecx, si;\n"
        "mov     edx, [esp+ecx*4+0x104];\n"
        "push    edx;\n"
        "lea eax, [esp+0xC];\n"
        "push    eax;\n"
        "call %[stricmp];\n"
        "add     esp, 8;\n"
        "test    eax, eax;\n"
        "jz whatever;\n"
        "dec     esi;\n"
        "cmp     si, bx;\n"
        "jg loop;\n"

    "not_recognized_by_halo:\n"
        "push %[input];\n"
        "call %[eval_cmd];\n"
        "add esp, 4;\n"

        "cmp %[cancel], 0;\n"
        "jne ret_cancel;\n"
        "jmp %[j_return_continue];\n"
    "ret_cancel:\n"
        "jmp %[j_return_cancel];\n"
    "whatever:\n"
        "jmp %[whatevertf];\n"
        : [eval_cmd] "+m" (func_evaluate_command)
        : [cancel] "m" (cancel_default_routine),
          [j_return] "m" (jmp_return_addr),
          [j_return_cancel] "m" (jmp_return_cancel),
          [j_return_continue] "m" (jmp_return_continue),
          [input] "m" (console_input_ptr),
          [whatevertf] "m" (whateverthefuck),
          [stricmp] "m" (func_stricmp)
    );
}

void init_command_in_hook(){
    uintptr_t sig_addr = sig_console_command_in_client.get_address();
    if (!sig_addr){
        sig_addr = sig_console_command_in_server.get_address();
    };
    uintptr_t sig_addr2 = sig_console_command_in_not_found.get_address();
    uintptr_t sig_addr3 = sig_console_input.get_address();
    uintptr_t sig_addr4 = sig_console_command_kavaworkaround.get_address();
    if (!command_in_patch.is_built() && sig_addr && sig_addr2 && sig_addr3 && sig_addr4){
        command_in_patch.build(sig_addr, 5, JMP_PATCH, (uintptr_t)&console_in_hook);
        jmp_return_addr = command_in_patch.get_return_address();
        jmp_return_continue = sig_addr2;
        jmp_return_cancel = sig_addr2+0x13;
        console_input_ptr = *(uintptr_t*)sig_addr3;
        whateverthefuck = sig_addr4;
    };

    if (command_in_patch.is_built()){
        command_in_patch.apply();
    };
}

void revert_command_in_hook(){
    command_in_patch.revert();
}

static Signature(false, sig_auto_complete_hook,
    {0x0F, 0xBF, 0x15, -1, -1, -1, -1, 0x68, -1, -1, -1, -1, 0x6A, 0x04, 0x52, 0x55, 0xE8});
static Signature(false, sig_auto_complete_collected_list,
    {-1, -1, -1, -1, 0x0F, 0xBF, 0xC8, 0x66, 0x40, 0x89, 0x34, 0x8A, 0x66, 0xA3,
     -1, -1, -1, -1, 0x83, 0xC3, 0x04, 0x4D, 0x75, 0x98, 0x5F});


static Patch(auto_complete_patch);

static intptr_t func_auto_complete = (intptr_t)&auto_complete;

static intptr_t results_ptr;
static intptr_t count_ptr;

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
    uintptr_t sig_addr3 = sig_auto_complete_collected_list.get_address();

    if (sig_addr && sig_addr3){
        auto_complete_patch.build(sig_addr, 7, CALL_PATCH, (uintptr_t)&auto_complete_hook);
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
    init_command_in_hook();
    init_command_auto_complete_hook();
}

void revert_console_hooks(){
    revert_command_in_hook();
    revert_command_auto_complete_hook();
}
/*
tab completion flags 0x14
A3????????750AC705

Tab Completion 0 -- place our call here
0FBF15????????68????????6A045255E8
*/
