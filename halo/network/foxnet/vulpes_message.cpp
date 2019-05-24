/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include "../../functions/message_delta.hpp"
#include "../../../includes/string_raw_data_encoder.hpp"
#include <cstring>

void send_vulpes_message(VulpesMessage* msg){
    HudChat message;
    wchar_t output[512];
    output[0] = msg->payload_size;
    size_t wchar_array_size = msg->payload_size/2 + msg->payload_size%2;
    wstr_raw_data_encode(&output[1], &msg->payload16[0], wchar_array_size);
    message.message = &output[0];
    message.msg_type = HudChatType::VULPES;
    message.player_id = -1;
    uint8_t buffer[1024];
    uint32_t size = mdp_encode_stateless_iterated(buffer, HUD_CHAT, &message);
    send_delta_message_to_all(&buffer, size, true, true, false, true, 3);
}

void handle_vulpes_message(VulpesMessage* msg){
}

void handle_hud_chat_vulpes_message(const wchar_t* msg){
    VulpesMessage output;
    output.payload_size = msg[0];
    size_t wchar_array_size = output.payload_size/2 + output.payload_size%2;
    wstr_raw_data_decode(&output.payload16[0], &msg[1], wchar_array_size);
    handle_vulpes_message(&output);
}
