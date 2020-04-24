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

#pragma once

#include <vulpes/memory/network/message_delta.hpp>
#include <vulpes/memory/network/message_delta_meta.hpp>

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
