#include "hooker.hpp"
#include <cstdint>
#include <string.h>
#include "../halo_functions/console.hpp"
#include "../command/handler.hpp"

// This area in the function is only executed when Halo doesn't recognize a command.
static Signature(true, sig_console_command_in,
    {0xE8, -1, -1, -1, -1, 0x8D, 0x4C, 0x24, 0x10, 0x6A, 0x20, 0x33, 0xDB, 0x51, 0x88, 0x9C,
     0x24, 0x17, 0x01, 0x00, 0x00});
static Signature(false, sig_console_log,
    {0x66, 0xA3, -1, -1, -1, -1, 0x0F, 0xBF, 0xC0, 0x69, 0xC0, 0xFF, 0x00, 0x00, 0x00, 0x8B,
     0xD7, 0x05, -1, -1, -1, -1, 0x8D, 0x64, 0x24, 0x00});
static Signature(false, sig_console_log_inc,
    {0x0F, 0xBF, 0x05, -1, -1, -1, -1, 0x40, 0x83, 0xF8, 0x08, 0x66, 0xC7, 0x05,
     -1, -1, -1, -1, 0x08, 0x00});

static Patch(command_in_patch);

char* console_log_buffer;
uint16_t* console_log_id;
uint16_t* logged_entries;

bool cancel_default_routine = false;

__attribute__((cdecl))
void evaluate_command(char* to, const char* input, int32_t max_size){
    cancel_default_routine = false;
    // emulating normal Halo behavior
    strncpy(to, input, max_size);
    // fooox
    printf("%s %s", to, input);
    if (strncmp("fox", input, max_size) == 0){
        printf("%s", "caught");
        console_out("EEEEEEEEP!");
        // Only execute this if the log pointers are valid.
        if (console_log_id && logged_entries){
            // Update the command log entry number. Cap off at 8, only 8 can be saved.
            *console_log_id = (*console_log_id+1) % 8;
            // Copy the current command to the log.
            strncpy(console_log_buffer+max_size*(*console_log_id), input, max_size);
            // Update logged command count if necessary.
            uint16_t logged_entry_count = *logged_entries+1;
            if (logged_entry_count <= 8){
                *logged_entries = logged_entry_count;
            };
        cancel_default_routine = true;
        };
    };
}

static intptr_t func_halo_strncpy;
static intptr_t func_evaluate_command = (intptr_t)&evaluate_command;
static intptr_t jmp_return_addr;

__attribute__((naked))
void console_in_hook(){
    asm (
        "pushad;\n"
        "push 0xFF;\n"
        "push edi;\n"
        "push eax;\n"
        "call %0;\n"
        "add esp, 0xC;\n"
        "cmp %1, 0;\n"
        "popad;\n"
        "jne cancel_return;\n"
        "ret;\n"
    "cancel_return:\n"
        // DANGER!
        // Removing return address from the stack
        "add esp, 4;\n"
        // Removing all the inputs from the stack
        "add esp, 0x10;\n"
        // Bringing the stack to the place where it started in the function that called this.
        "add esp, 0x500;\n"
        // returning to the place that called the function that called this function.
        "ret;\n"
        //"jmp %1;\n"
        : "+m" (func_evaluate_command)
        : "m" (cancel_default_routine)
        //: "m" (jmp_return_addr)
    );
}

void init_command_in_hook(){
    intptr_t sig_addr  = sig_console_command_in.get_address();
    intptr_t sig_addr2 = sig_console_log.get_address();
    intptr_t sig_addr3 = sig_console_log_inc.get_address();

    if (!command_in_patch.is_built() && sig_addr){
        func_halo_strncpy = get_call_address(sig_addr);
        command_in_patch.build(sig_addr, 5, CALL_PATCH, (uintptr_t)&console_in_hook);
        jmp_return_addr = command_in_patch.get_return_address();
        if (sig_addr2 && sig_addr3){
            console_log_buffer = (char*)*(intptr_t*)(sig_addr2+0x12);
            console_log_id = (uint16_t*)*(intptr_t*)(sig_addr2+2);
            logged_entries = (uint16_t*)*(intptr_t*)(sig_addr3+3);
        }else{
            console_out_error("Error: Couldn't find console log info. Vulpes Commands will not be logged.");
        };
    };

    if (command_in_patch.is_built()){
        command_in_patch.apply();
    };
}

void revert_command_in_hook(){
    command_in_patch.revert();
}

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
