/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <hooker/hooker.hpp>

#include <vulpes/network/foxnet/vulpes_message.hpp>
#include <vulpes/memory/signatures.hpp>

#include "incoming_packets.hpp"

DEFINE_EVENT_HOOK_LIST(EVENT_RECEIVE_CHAT_MESSAGE, hud_chat_events);

// HUD_CHAT = 0xF
extern "C" bool process_hud_chat_message(HudChat* packet) {
    if (packet->msg_type == HudChatType::VULPES) {
        handle_hud_chat_vulpes_message(packet->message);
        return false;
    }
    // We don't want to touch these.
    if (packet->msg_type == HudChatType::HAC ||
        packet->msg_type == HudChatType::HCN) {
        return true;
    }
    bool allow = true;
    call_in_order_allow(hud_chat_events, allow, packet);
    return allow;
}

extern "C" {
    uintptr_t incoming_packets__jmp_hud_chat_original_code;
    extern incoming_packets__hook_hud_chat_intercept();
}

Patch(
    hud_chat_hook, 0, 8,
    JMP_PATCH, &incoming_packets__hook_hud_chat_intercept
);

void init_hud_chat_hook() {
    hud_chat_hook.build(sig_hook_hud_chat());
    incoming_packets__jmp_hud_chat_original_code =
        hud_chat_hook.return_address();
    hud_chat_hook.apply();
}

void revert_hud_chat_hook() {
    hud_chat_hook.revert();
}

void init_incoming_packet_hooks() {
    init_hud_chat_hook();
}

void revert_incoming_packet_hooks() {
    revert_hud_chat_hook();
}
