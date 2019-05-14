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

#pragma pack(push, 1)

struct StrCallJmp {
    uint8_t intruction_byte;
    intptr_t offset;
};
struct ProtectRegisters {
    InstructionBytes one = PUSHFD_BYTE;
    InstructionBytes two = PUSHAD_BYTE;
};
struct RevertRegisters {
    InstructionBytes one = POPAD_BYTE;
    InstructionBytes two = POPFD_BYTE;
};

struct Cave {
    ProtectRegisters beforeprot;
    StrCallJmp before;
    RevertRegisters rev1;
    StrCallJmp func;
    StrCallJmp func2;
    ProtectRegisters afterprot;
    StrCallJmp after;
    RevertRegisters rev2;
    StrCallJmp back;
};

#pragma pack(pop)
