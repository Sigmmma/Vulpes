#pragma once
#include "../memory/message_delta_meta.hpp"
#include "../memory/message_delta.hpp"

// Do not use this until all args are named.
uint32_t mdp_encode_stateless_iterated(
    void* output_buffer, int32_t arg1, MessageDeltaType type,
    uint32_t arg3, void* unencoded_message, uint32_t arg5, uint32_t arg6, uint32_t arg7);

uint32_t mdp_encode_stateless_iterated(
    void* output_buffer, MessageDeltaType type, void* unencoded_message);

bool mdp_decode_stateless_iterated(void* destination, MessageDeltaHeader* message_header);

void mdp_discard_iteration_body(MessageDeltaHeader* message_header);

void send_delta_message_to_all(void* message, uint32_t message_size,
    bool ingame_only, bool write_to_local_connection,
    bool flush_queue, bool unbuffered, int32_t buffer_priority);

void send_delta_message_to_player(int32_t player_id, void* message, uint32_t message_size,
    bool ingame_only, bool write_to_local_connection,
    bool flush_queue, bool unbuffered, int32_t buffer_priority);

void send_delta_message_to_all_except(int32_t player_id, void* message, uint32_t message_size,
    bool ingame_only, bool write_to_local_connection,
    bool flush_queue, bool unbuffered, int32_t buffer_priority);

void init_message_delta_processor();
void init_message_delta_sender();
