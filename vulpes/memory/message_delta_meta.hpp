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

#include "types.hpp"

struct MessageDeltaHeader {
    //MessageDeltaDecodingInfo* decoding_information;
    void* decoding_information;
    int32_t number;
};
/*
struct MessageDeltaDecodingInfo {
    MessageDeltaMode mode; // 0 == stateless, 1 == incremental
    MessageDeltaType definition_type;
    PAD(3);
    int32_t current_iteration;
    int32_t state;
    char* input_stream;
    int32_t current_bit; // maybe?
    bool iteration_header_decoded;
    PAD(1);
    bool iteration_body_decoded;
    PAD(1);
};

struct MessageDeltaFieldTypeDefinition {
    int32_t type; // 0
    //char name [0x4B]; // 4
    bool requires_parameters; // 4F
    int32_t maximum_size_calculator; // 50
    bool verify_parameters; // 54
    PAD(3);
    int32_t field_58; // 58
    bool initialized; // 5C
    PAD(3);
}; //static_assert(sizeof(MessageDeltaFieldTypeDefinition) == 0x60);

struct MessageDeltaFieldPropertyDefinition {
    int32_t type; // 0
    char name[0x4C] = "name"; // 4
    void* encoder; // 50
    void* decoder; // 54
    int32_t* parameters; // 58
    int32_t max_size; // 5C
    int32_t field_60; // 60
    bool initialized; // 64
    PAD(3);
}; //static_assert(sizeof(MessageDeltaFieldPropertyDefinition) == 0x68);

struct MessageDeltaFieldReferenceDefinition {
    MessageDeltaFieldPropertyDefinition* properties;
    //char struct_name[0x4B];
    //char name[0x4D];
    int32_t offset; // structure offset to read/write from when encoding/decoding
    int32_t baseline_offset;
    bool initialized;
    PAD(3);
}; //static_assert(sizeof(MessageDeltaFieldPropertyDefinition) == 0xA8);

struct MessageDeltaFieldReferenceSet {
    int32_t field_count;
    int32_t max_data_size;
    //MessageDeltaFieldReferenceDefinition fields[];
};

struct MessageDeltaDefinition {
    //char name[0x50];
    int32_t type;
    int32_t field_50 = -1; // 4
    int32_t iteration_size = -1; // 8
    int32_t iteration_independent_header_size = -1; // C
    int32_t total_size = -1; // 10
    int32_t max_iterations = 1; // 14
    bool initialized = false; // 18
    PAD(3);
    int32_t* header_field_set; // 1C // this is the sapien pointer
    struct {
        int32_t field_count; // 20
        int32_t max_data_size = -1; // 24
    }body_field_set;
};
*/
