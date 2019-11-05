/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#pragma once

#include <cstdint>

enum InstructionBytes : uint8_t {
    NOP_BYTE  = 0x90,
    CALL_BYTE = 0xE8,
    JMP_BYTE  = 0xE9,
    JMP_SMALL_BYTE = 0xEB,

    PUSHFD_BYTE = 0x9C,
    PUSHAD_BYTE = 0x60,
    POPFD_BYTE  = 0x91,
    POPAD_BYTE  = 0x61,
    PUSH_EDI_BYTE = 0x57,

    // 2 byte conditional jumps
    CONDJ_BYTE = 0x0F, // first byte

    JA_BYTE    = 0x87
};

// Here are macros for the code parts of the template based CodePatch initializers.
// These are made because templates cannot have their code in
// a source file while having the template definition in the header.

#define CPTEMPLINIT1 {\
    name = d_name; sig = p_sig; offset = p_sig_offset;\
    patch_size = p_size; type = p_type;\
    setup_internal(reinterpret_cast<void*>(&content), sizeof(T));\
}

#define CPTEMPLINIT2 {\
    name = d_name; patch_address = p_address;\
    patch_size = p_size; type = p_type;\
    setup_internal(reinterpret_cast<void*>(&content), sizeof(T));\
}
