#pragma once
#include <cstdint>

void send_delta_message_to_all(void* message, uint32_t message_size,
    bool ingame_only, bool write_to_local_connection,
    bool flush_queue, bool unbuffered, int32_t buffer_priority);

void send_delta_message_to_player(int32_t player_id, void* message, uint32_t message_size,
    bool ingame_only, bool write_to_local_connection,
    bool flush_queue, bool unbuffered, int32_t buffer_priority);

void send_delta_message_to_all_except(int32_t player_id, void* message, uint32_t message_size,
    bool ingame_only, bool write_to_local_connection,
    bool flush_queue, bool unbuffered, int32_t buffer_priority);

void init_message_delta_sender();
