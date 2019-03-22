#include <stdint.h>
#include "hooks/shared_asm.hpp"



uint16_t* process_hud_chat_message(HudChatType msg_type, int32_t player_id, uint16_t* msg_text) {
    bool return_to_original_code = true;

    return msg_text;
}

// Address to jump back to if we have determined that the hud_message should be processed as normal.
static uint32_t jmp_original_code;
static uint32_t func_process_hud_chat_message = reinterpret_cast<uint32_t>(&process_hud_chat_message);

__attribute__((naked))
void hook_hud_chat_intercept(){

    asm (
        // Copy original code behavior we are overwriting.
        "test al, al;\n"
        "jz abort;\n"
        // Our own code starts here.
        "call save_registers;\n"
        // Put the stack pointer into ebx before we start pushing.
        "mov ebx, esp;\n"
        // Call process_packet()
        "push [ebx+0x10+0xC];\n" // text_pointer
        "push [ebx+0x10+0x10];\n"// player_id
        "push [ebx+0x10+0x14];\n"// message_type
        "call %[process_hud_chat_message];\n"
        // Move the possibly manipulated text_pointer into the address that halo will look for it in.
        // DO NOT dynamically allocate a string for this. Only use staticly allocated ones.
        "add esp, 12;\n"
        "mov [esp+0x10+0xC], eax;\n"
        // If it returns false, cancel the original function by returning.
        // If it returns true, go back to the original function.
        "cmp eax, 0;\n"
        "jne revert_to_original_code;\n"

        "call restore_registers;\n"
        // Copy of the original way the function aborts.
        // This is so we can abort the game's processing of this packet.
        "abort:\n"
        "pop edi;\n"
        "pop esi;\n"
        "pop ebx;\n"
        "mov esp, ebp;\n"
        "pop ebp;\n"
        "ret\n;"
        // Restore the original registers.
        "revert_to_original_code:\n"
        "call restore_registers;\n"
        "jmp %[original_code];\n"
        : [process_hud_chat_message] "+m" (func_process_hud_chat_message)
        : [original_code] "m" (jmp_original_code)
    );
}

int16_t hud_chat_hook_signature_bytes[] = { 0x84, 0xC0, 0x0F, 0x84, -1, -1, -1, -1, 0x8A, 0x44, 0x24, 0x10, 0x3C, 0xFF, 0x0F, 0x84 };
CodeSignature hud_chat_hook_signature(true, hook_signature_bytes);
CodePatch hud_chat_hook(hud_chat_hook_signature.get_address(), 8, JMP_PATCH, (uintptr_t)hook_hud_chat_intercept);
