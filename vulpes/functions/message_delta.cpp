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

#include <vulpes/memory/signatures.hpp>

#include "message_delta.hpp"

// TODO: Clean up function pointers that are passed to code.

extern "C" {
    uintptr_t func_mdp_encode_ptr;
    uintptr_t func_mdp_decode_ptr;
    uintptr_t func_mdp_discard_ptr;
}

uint32_t mdp_encode_stateless_iterated(
    void* output_buffer, int32_t arg1, MessageDeltaType type,
    uint32_t arg3, void* unencoded_message, uint32_t arg5, uint32_t arg6, uint32_t arg7) {

    uint32_t output_size;
    int32_t type_int = type;
    void** ptr_to_ptr = &unencoded_message;
    asm (
        "pushad;"
        "push %8;"
        "push %7;"
        "push %6;"
        "push %5;"
        "push %4;"
        "push %3;"
        "push %2;"
        "mov edx, 0x7FF8;"
        "mov eax, %1;"
        "call [_func_mdp_encode_ptr];"
        "add esp, 0x1C;"
        "mov %9, eax;"
        "popad;"
        : "+m" (func_mdp_encode_ptr)
        : "m" (output_buffer),
          "m" (arg1),
          "m" (type_int),
          "m" (arg3),
          "m" (ptr_to_ptr),
          "m" (arg5),
          "m" (arg6),
          "m" (arg7),
          "m" (output_size)
    );
    return output_size;
}

uint32_t mdp_encode_stateless_iterated(
    void* output_buffer, MessageDeltaType type, void* unencoded_message) {

    return mdp_encode_stateless_iterated(output_buffer, 0, type, 0, unencoded_message, 0, 1, 0);
}
// Test this // Wrong, eax is also an input for the type.
bool mdp_decode_stateless_iterated(void* destination, MessageDeltaHeader* message_header) {
    bool success;
    asm (
        "mov ecx, %1;"
        "mov eax, %2;"
        "call [_func_mdp_decode_ptr];"
        "mov %3, al;"
        : "+m" (func_mdp_decode_ptr)
        : "m" (destination),
          "m" (message_header),
          "m" (success)
    );
    return success;
}
// Test this
void mdp_discard_iteration_body(MessageDeltaHeader* message_header) {
    asm (
        "pushad;"
        "mov eax, %1;"
        "call [_func_mdp_discard_ptr];"
        "popad;"
        :
        : "m" (func_mdp_discard_ptr), "m" (message_header)
    );
}

void init_message_delta_processor() {
    func_mdp_encode_ptr = sig_func_mdp_decode_stateless_iterated();
    func_mdp_decode_ptr = sig_func_mdp_encode_stateless_iterated();
    func_mdp_discard_ptr = sig_func_mdp_discard_iteration_body();
}

extern "C" {
    uintptr_t* socket_ready_ptr;
    uintptr_t func_send_message_to_all_ptr;
    uintptr_t func_send_message_to_player_ptr;
}

void send_delta_message_to_all(void* message, uint32_t message_size,
    bool ingame_only, bool write_to_local_connection,
    bool flush_queue, bool unbuffered, int32_t buffer_priority) {

    if(*socket_ready_ptr) {
        int32_t bool_ingame_only = ingame_only;
        int32_t bool_write_to_local_connection = write_to_local_connection;
        int32_t bool_flush_queue = flush_queue;
        int32_t bool_unbuffered = unbuffered;

        asm (
            "push ecx;"
            "push eax;"

            "push %8;"
            "push %7;"
            "push %6;"
            "push %5;"
            "push %2;"
            "push %4;"
            "mov ecx, [_socket_ready_ptr];"
            "mov ecx, [ecx];"
            "mov eax, %3;"
            "call [_func_send_message_to_all_ptr];"
            "add esp, 0x18;"

            "pop eax;"
            "pop ecx;"
            :
            : "m" (func_send_message_to_all_ptr),   // 0
              "m" (socket_ready_ptr),               // 1
              "m" (message),                    // 2
              "m" (message_size),               // 3
              "m" (bool_ingame_only),           // 4
              "m" (bool_write_to_local_connection),//5
              "m" (bool_flush_queue),           // 6
              "m" (bool_unbuffered),            // 7
              "m" (buffer_priority)             // 8
        );
    }
}

void send_delta_message_to_player(int32_t player_id, void* message, uint32_t message_size,
    bool ingame_only, bool write_to_local_connection,
    bool flush_queue, bool unbuffered, int32_t buffer_priority) {

    if(*socket_ready_ptr) {
        int32_t bool_ingame_only = ingame_only;
        int32_t bool_write_to_local_connection = write_to_local_connection;
        int32_t bool_flush_queue = flush_queue;
        int32_t bool_unbuffered = unbuffered;

        asm (
            "push esi;"
            "push eax;"

            "push %8;"
            "push %7;"
            "push %6;"
            "push %5;"
            "push %3;"
            "push %2;"
            "push %4;"
            "mov esi, [_socket_ready_ptr];"
            "mov esi, [esi];"
            "mov eax, %9;"
            "call [_func_send_message_to_player_ptr];"
            "add esp, 0x1C;"

            "pop eax;"
            "pop esi;"
            :
            : "m" (func_send_message_to_player_ptr),// 0
              "m" (socket_ready_ptr),               // 1
              "m" (message),                    // 2
              "m" (message_size),               // 3
              "m" (bool_ingame_only),           // 4
              "m" (bool_write_to_local_connection),//5
              "m" (bool_flush_queue),           // 6
              "m" (bool_unbuffered),            // 7
              "m" (buffer_priority),            // 8
              "m" (player_id)                   // 9
        );
    }
}

void send_delta_message_to_all_except(int32_t player_id, void* message, uint32_t message_size,
    bool ingame_only, bool write_to_local_connection,
    bool flush_queue, bool unbuffered, int32_t buffer_priority) {

    for (int i = 0; i<16; i++) {
        if (i != player_id) {
            send_delta_message_to_player(i, message, message_size,
                ingame_only, write_to_local_connection,
                flush_queue, unbuffered, buffer_priority);
        }
    }
}

void init_message_delta_sender() {
    socket_ready_ptr = *sig_func_net_send_message_socket_ready();
    func_send_message_to_all_ptr = sig_func_net_send_message_to_all();
    func_send_message_to_player_ptr = sig_func_net_send_message_to_player();
}
