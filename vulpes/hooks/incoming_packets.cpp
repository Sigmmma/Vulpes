/*
 * This file is part of Vulpes, an extension of Halo Custom Edition's capabilities.
 * Copyright (C) 2019-2020 gbMichelle (Michelle van der Graaf)
 *
 * Vulpes is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, version 3.
 *
 * Vulpes is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * long with Vulpes.  If not, see <https://www.gnu.org/licenses/agpl-3.0.en.html>
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

static Patch(
    hud_chat_hook, NULL, 8,
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
