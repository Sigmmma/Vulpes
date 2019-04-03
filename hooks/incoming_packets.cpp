#include "incoming_packets.hpp"

#include <cstdint>
#include "../network/message_delta_definitions.hpp"
#include "hooker.hpp"

// This file is going to be big, so let's talk about structure.
// At the top level we're going to have our functions that call places outside of this source file.
// These will be called by our hook assembly, so we need to keep track of their inputs and outputs.
//
// Below that we're going to have all of our code that actually hooks into the game.
//      1. C variables used in the assembly code.
//      2. The assembly code.
//      3. The signature and code patch object.
//
// At the bottom will be our initialization code, this will run all the signature scans,
// and populate return jump addresses, do any specific setup.
//
// We will also have our deinitialization code below that,
// this will clean up any of our edits to the game's code.
// Even if we don't expect that we'll need it.

////// Connection functions.

uintptr_t process_hud_chat_message(HudChatType msg_type, int32_t player_id, uint16_t* msg_text) {
    bool return_to_original_code = true;

    if (return_to_original_code){
        return (uintptr_t)msg_text;
    }
    return 0;
}

////// Hook code

// HUD_CHAT = 0xF
uintptr_t jmp_hud_chat_original_code;
uintptr_t func_process_hud_chat_message = (uintptr_t)&process_hud_chat_message;

__attribute__((naked))
void hook_hud_chat_intercept(){
    asm (
        // Copy original code behavior we are overwriting.
        "test al, al;\n"
        "jz abort;\n"
        // Our own code starts here.
        "push eax;\n"
        "push ebx;\n"
        "push ecx;\n"
        "push edx;\n"
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

        "pop edx;\n"
        "pop ecx;\n"
        "pop ebx;\n"
        "pop eax;\n"
        "jne revert_to_original_code;\n"
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
        ///"call restore_registers;\n"
        "jmp %[hud_chat_original_code];\n"
        : [process_hud_chat_message] "+m" (func_process_hud_chat_message)
        : [hud_chat_original_code] "m" (jmp_hud_chat_original_code)
    );
}

Signature(true, hud_chat_hook_signature,
    {0x84, 0xC0, 0x0F, 0x84, -1, -1, -1, -1, 0x8A, 0x44, 0x24, 0x10, 0x3C, 0xFF, 0x0F, 0x84 });
CodePatch hud_chat_hook(hud_chat_hook_signature.get_address(),
    8, JMP_PATCH, reinterpret_cast<uintptr_t>(&hook_hud_chat_intercept));

void init_hud_chat_hook(){
    jmp_hud_chat_original_code = hud_chat_hook.get_return_address();
    hud_chat_hook.apply();
}

void revert_hud_chat_hook(){
    hud_chat_hook.revert();
}

const std::vector<int16_t> biped_new_hook_signature_bytes = { 0x3C, 0x01, 0x0F, 0x85, -1, -1, -1, -1, 0x8D, 0x54, 0x24, 0x28 };
const std::vector<int16_t> vehicle_new_hook_signature_bytes = { 0x3C, 0x01, 0x0F, 0x85, -1, -1, -1, -1, 0x8D, 0x54, 0x24, 0x40 };
const std::vector<int16_t> equipment_new_hook_signature_bytes = { 0x3C, 0x01, 0x0F, 0x85, -1, -1, -1, -1, 0x8D, 0x54, 0x24, 0x3C };
const std::vector<int16_t> weapon_new_hook_signature_bytes = { 0x3C, 0x01, 0x0F, 0x85, -1, -1, -1, -1, 0x8D, 0x54, 0x24, 0x34 };
const std::vector<int16_t> unit_kill_hook_signature_bytes = { 0x84, 0xC0, 0x0F, 0x84, -1, -1, -1, -1, 0x8B, 0x44, 0x24, 0x08, 0x85, 0xC0, 0x0F, 0x84, -1, -1, -1, -1, 0x8B, 0x15, -1, -1, -1, -1, 0x8B, 0x4A, 0x28, 0x8B, 0x3C, 0x81, 0x83, 0xFF, 0xFF, 0x0F, 0x84, -1, -1, -1, -1, 0x8A, 0x54, 0x24, 0x12 };
const std::vector<int16_t> damage_dealt_hook_signature_bytes = { 0x84, 0xC0, 0x0F, 0x84, -1, -1, -1, -1, 0x8B, 0x44, 0x24, 0x08, 0x85, 0xC0, 0x0F, 0x84, -1, -1, -1, -1, 0x8B, 0x15, -1, -1, -1, -1, 0x8B, 0x4A, 0x28, 0x8B, 0x3C, 0x81, 0x83, 0xFF, 0xFF, 0x0F, 0x84, -1, -1, -1, -1, 0x6A, 0x03 };


void init_incoming_packet_hooks(){
    init_hud_chat_hook();
}

void revert_incoming_packet_hooks(){
    revert_hud_chat_hook();
}
