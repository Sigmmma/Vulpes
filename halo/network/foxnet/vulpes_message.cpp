/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include "../../functions/message_delta.hpp"
#include "../../../includes/string_raw_data_encoder.hpp"
#include "../../../includes/bit_buffer.hpp"
#include <cstring>

void send_vulpes_message(VulpesMessage* msg){
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

void handle_vulpes_message(VulpesMessage* msg){
}

void handle_hud_chat_vulpes_message(const wchar_t* msg){
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
