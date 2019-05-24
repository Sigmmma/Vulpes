/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
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
