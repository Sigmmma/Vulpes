#pragma once
#include "definition.hpp"
#include "enums.hpp"

// Do not use this until all args are named.
uint32_t mdp_encode_stateless_iterated(
    void* output_buffer, int32_t arg1, MessageDeltaType type,
    uint32_t arg3, void* unencoded_message, uint32_t arg5, uint32_t arg6, uint32_t arg7);

uint32_t mdp_encode_stateless_iterated(
    void* output_buffer, MessageDeltaType type, void* unencoded_message);

bool mdp_decode_stateless_iterated(void* destination, MessageDeltaHeader* message_header);

void mdp_discard_iteration_body(MessageDeltaHeader* message_header);
