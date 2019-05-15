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
