#include "incoming_packets.hpp"

#include <cstdint>
#include "../network/message_delta_definitions.hpp"
#include "hooker.hpp"
#include <cstdio>

// HUD_CHAT = 0xF
bool process_hud_chat_message(HudChat* packet){
    if (packet->msg_type == HudChatType::VULPES){
        return false;
    };
    // We don't want to touch these.
    if (packet->msg_type == HudChatType::HAC ||
        packet->msg_type == HudChatType::HCN){
        return true;
    };

    return true;
}

static uintptr_t jmp_hud_chat_original_code;
static uintptr_t func_process_hud_chat_message = (uintptr_t)&process_hud_chat_message;

__attribute__((naked))
void hook_hud_chat_intercept(){
    asm (
        // Copy original code behavior we are overwriting.
        "cmp al, 0;\n"
        "je abort;\n"
        // Our own code starts here.
        "push eax;\n"
        "push ebx;\n"
        "push ecx;\n"
        "push edx;\n"
        // Call process_packet()
        "lea ebx, [esp+0x10+0xC];\n"
        "push ebx;\n"
        "call %[process_hud_chat_message];\n"
        "add esp, 4;\n"
        // If it returns false, cancel the original function by returning.
        // If it returns true, go back to the original function.
        "cmp al, 0;\n"
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
        "jmp %[hud_chat_original_code];\n"
        : [process_hud_chat_message] "+m" (func_process_hud_chat_message)
        : [hud_chat_original_code] "m" (jmp_hud_chat_original_code)
    );
}

static Signature(true, hud_chat_hook_signature,
    {0x84, 0xC0, 0x0F, 0x84, -1, -1, -1, -1, 0x8A, 0x44, 0x24, 0x10, 0x3C, 0xFF, 0x0F, 0x84 });
static Patch(hud_chat_hook);

void init_hud_chat_hook(){
    hud_chat_hook.build(hud_chat_hook_signature.get_address(), 8, JMP_PATCH, reinterpret_cast<uintptr_t>(&hook_hud_chat_intercept));
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
