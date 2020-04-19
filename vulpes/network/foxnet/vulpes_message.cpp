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

#include <cstring>

#include <util/string_raw_data_encoder.hpp>
#include <util/bit_buffer.hpp>

#include <vulpes/functions/message_delta.hpp>

#include "vulpes_message.hpp"

void send_vulpes_message(VulpesMessage* msg) {
    uint8_t buffer[1024]; // Final packet buffer.
    wchar_t output[1024/2]; // Pre-processed chat packet buffer.

    // Prepare message for chat packet encoding.
    output[0] = msg->payload_size;
    // Encode the data so it doesn't contain any null chars.
    wstr_raw_data_encode(
        &output[1],
        &msg->payload16[0],
        msg->payload_size/2 + msg->payload_size%2
    );
    // Pre-processed chat packet.
    HudChat message(HudChatType::VULPES, -1, &output[0]);
    // Encode chat packet for sending.
    uint32_t size = mdp_encode_stateless_iterated(buffer, HUD_CHAT, &message);
    // Send chat packet.
    send_delta_message_to_all(&buffer, size, true, true, false, true, 3);
}

void handle_vulpes_message(VulpesMessage* msg) {
}

void handle_hud_chat_vulpes_message(const wchar_t* msg) {
    VulpesMessage output; // Vulpes message.

    // Set up the size for the message handler.
    output.payload_size = msg[0];
    // Decode the data to get back any null bytes.
    wstr_raw_data_decode(
        &output.payload16[0],
        &msg[1],
        output.payload_size/2 + output.payload_size%2
    );
    // Actually handle the data in the message.
    handle_vulpes_message(&output);
}
