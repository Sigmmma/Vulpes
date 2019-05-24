/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include "incoming_packets.hpp"
#include "../../hooker/hooker.hpp"
#include "../network/foxnet/vulpes_message.hpp"

DEFINE_EVENT_HOOK_LIST(EVENT_RECEIVE_CHAT_MESSAGE, hud_chat_events);

// HUD_CHAT = 0xF
bool process_hud_chat_message(HudChat* packet){
    if (packet->msg_type == HudChatType::VULPES){
        handle_hud_chat_vulpes_message(packet->message);
        return false;
    };
    // We don't want to touch these.
    if (packet->msg_type == HudChatType::HAC ||
        packet->msg_type == HudChatType::HCN){
        return true;
    };
    bool allow = true;
    call_in_order_allow(hud_chat_events, allow, packet);
    return allow;
}

static uintptr_t jmp_hud_chat_original_code;
static auto func_process_hud_chat_message = &process_hud_chat_message;

__attribute__((naked))
void hook_hud_chat_intercept(){
    asm (
        // Copy original code behavior we are overwriting.
        "cmp al, 0;"
        "je abort;"
        // Our own code starts here.
        "push eax;"
        "push ebx;"
        "push ecx;"
        "push edx;"
        // Call process_packet()
        "lea ebx, [esp+0x10+0xC];"
        "push ebx;"
        "call %[process_hud_chat_message];"
        "add esp, 4;"
        // If it returns false, cancel the original function by returning.
        // If it returns true, go back to the original function.
        "cmp al, 0;"
        "pop edx;"
        "pop ecx;"
        "pop ebx;"
        "pop eax;"
        "jne revert_to_original_code;"
        // Copy of the original way the function aborts.
        // This is so we can abort the game's processing of this packet.
    "abort:"
        "pop edi;"
        "pop esi;"
        "pop ebx;"
        "mov esp, ebp;"
        "pop ebp;"
        "ret;"
        // Restore the original registers.
    "revert_to_original_code:"
        "jmp %[hud_chat_original_code];"
        :
        : [process_hud_chat_message] "m" (func_process_hud_chat_message),
          [hud_chat_original_code] "m" (jmp_hud_chat_original_code)
    );
}

Signature(true, hud_chat_hook_signature,
    {0x84, 0xC0, 0x0F, 0x84, -1, -1, -1, -1, 0x8A, 0x44, 0x24, 0x10, 0x3C, 0xFF, 0x0F, 0x84 });
Patch(hud_chat_hook, hud_chat_hook_signature, 0, 8, JMP_PATCH, &hook_hud_chat_intercept);

void init_hud_chat_hook(){
    hud_chat_hook.build();
    jmp_hud_chat_original_code = hud_chat_hook.return_address();
    hud_chat_hook.apply();
}

void revert_hud_chat_hook(){
    hud_chat_hook.revert();
}
/*
static intptr_t func_handle_vulpes_message = (intptr_t)&handle_vulpes_message;
static intptr_t network_related_bool_ptr;

// We replace the packet index out of bounds condition with our own code here
__attribute__((naked))
void case57(){
    asm (
        "cmp edx, 57;"
        "jne not_vulpes"
        "push esi;"
        "call %[handle_vulpes_message];"
        "add esp, 4;"
    "not_vulpes:"
        "mov esi, %[network_related_bool_ptr];"
        "mov BYTE PTR ds:[esi], 0;"
        "pop esi;"
        "ret;"
        : [network_related_bool_ptr] "+m" (network_related_bool_ptr)
        : [handle_vulpes_message] "m" (func_handle_vulpes_message)
    );
}

Signature(true, sig_vulpes_packet_receive_hook,
    {0x8B, 0x08, 0x88, 0x15, -1, -1, -1, -1, 0x8B, 0x51, 0x04, 0x83, 0xFA, 0x38, 0x0F, 0x87});
Signature(true, sig_network_related_bool,
    {-1, -1, -1, -1, 0x00, 0x5E, 0xC3, 0x56, 0xE8, -1, -1, -1, -1, 0x83, 0xC4, 0x04, 0xC6, 0x05});
Patch(vulpes_packet_receive);

void init_vulpes_message_hook(){
    vulpes_packet_receive.build_old(sig_vulpes_packet_receive_hook.address()+0xE,
        6, JA_PATCH, reinterpret_cast<uintptr_t>(&case57));
    vulpes_packet_receive.apply();
    network_related_bool_ptr = *reinterpret_cast<int32_t*>(sig_network_related_bool.address());
}

void revert_vulpes_message_hook(){
    vulpes_packet_receive.revert();
}

const std::vector<int16_t> biped_new_hook_signature_bytes = { 0x3C, 0x01, 0x0F, 0x85, -1, -1, -1, -1, 0x8D, 0x54, 0x24, 0x28 };
const std::vector<int16_t> vehicle_new_hook_signature_bytes = { 0x3C, 0x01, 0x0F, 0x85, -1, -1, -1, -1, 0x8D, 0x54, 0x24, 0x40 };
const std::vector<int16_t> equipment_new_hook_signature_bytes = { 0x3C, 0x01, 0x0F, 0x85, -1, -1, -1, -1, 0x8D, 0x54, 0x24, 0x3C };
const std::vector<int16_t> weapon_new_hook_signature_bytes = { 0x3C, 0x01, 0x0F, 0x85, -1, -1, -1, -1, 0x8D, 0x54, 0x24, 0x34 };
const std::vector<int16_t> unit_kill_hook_signature_bytes = { 0x84, 0xC0, 0x0F, 0x84, -1, -1, -1, -1, 0x8B, 0x44, 0x24, 0x08, 0x85, 0xC0, 0x0F, 0x84, -1, -1, -1, -1, 0x8B, 0x15, -1, -1, -1, -1, 0x8B, 0x4A, 0x28, 0x8B, 0x3C, 0x81, 0x83, 0xFF, 0xFF, 0x0F, 0x84, -1, -1, -1, -1, 0x8A, 0x54, 0x24, 0x12 };
const std::vector<int16_t> damage_dealt_hook_signature_bytes = { 0x84, 0xC0, 0x0F, 0x84, -1, -1, -1, -1, 0x8B, 0x44, 0x24, 0x08, 0x85, 0xC0, 0x0F, 0x84, -1, -1, -1, -1, 0x8B, 0x15, -1, -1, -1, -1, 0x8B, 0x4A, 0x28, 0x8B, 0x3C, 0x81, 0x83, 0xFF, 0xFF, 0x0F, 0x84, -1, -1, -1, -1, 0x6A, 0x03 };
*/

void init_incoming_packet_hooks(){
    init_hud_chat_hook();
    //init_vulpes_message_hook();
}

void revert_incoming_packet_hooks(){
    revert_hud_chat_hook();
    //revert_vulpes_message_hook();
}
