#include "message_delta_processor.hpp"
#include "../../hooks/hooker.hpp"

Signature(true, sig_mdp_decode_stateless_iterated,
    {0x57, 0x8B, 0x38, 0x51, 0x83, 0xC0, 0x04, 0x6A, 0x00, 0x50, 0xE8});

Signature(true, sig_mdp_encode_stateless_iterated,
    {0x81, 0xEC, 0xA4, 0x00, 0x00, 0x00, 0x53, 0x55, 0x8B, 0xAC, 0x24,
     0xBC, 0x00, 0x00, 0x00});

Signature(true, sig_mdp_discard_iteration_body,
    {0x81, 0xEC, 0x00, 0x08, 0x00, 0x00, 0x56, 0x57, 0x8B, 0x38, 0x8B, 0x37});

uint32_t mdp_encode_stateless_iterated(
    void* output_buffer, int32_t arg1, MessageDeltaType type,
    uint32_t arg3, void* unencoded_message, uint32_t arg5, uint32_t arg6, uint32_t arg7){

    uint32_t output_size;
    static uintptr_t func_mdp_encode = sig_mdp_encode_stateless_iterated.get_address();
    int32_t type_int = static_cast<int32_t>(type);
    asm (
        "push %8;\n"
        "push %7;\n"
        "push %6;\n"
        "push %5;\n"
        "push %4;\n"
        "push %3;\n"
        "push %2;\n"
        "mov edx, 0x7FF8;\n"
        "mov eax, %1;\n"
        "call %0;\n"
        "add esp, 0x1C;\n"
        "mov %9, eax;\n"
        : "+m" (func_mdp_encode)
        : "m" ((uint32_t)output_buffer),
          "m" (arg1),
          "m" (type_int),
          "m" (arg3),
          "m" ((uint32_t)unencoded_message),
          "m" (arg5),
          "m" (arg6),
          "m" (arg7),
          "m" (output_size)
    );
    return output_size;
}

uint32_t mdp_encode_stateless_iterated(
    void* output_buffer, MessageDeltaType type, void* unencoded_message){

    return mdp_encode_stateless_iterated(output_buffer, 0, type, 0, unencoded_message, 0, 1, 0);
}

bool mdp_decode_stateless_iterated(void* destination, void* message_header){
    bool success;
    static uintptr_t func_mdp_decode = sig_mdp_decode_stateless_iterated.get_address();
    asm (
        "mov ecx, %1;\n"
        "mov eax, %2;\n"
        "call %0;\n"
        "mov %3, al;\n"
        : "+m" (func_mdp_decode)
        : "m" (destination),
          "m" (message_header),
          "m" (success)
    );
    return success;
}

bool mdp_discard_iteration_body(void* message_header){
    bool success;
    static uintptr_t func_mdp_discard = sig_mdp_discard_iteration_body.get_address();
    asm (
        "mov eax, %1\n"
        "call %0;\n"
        "mov %2, al;\n"
        : "+m" (func_mdp_discard)
        : "m" (message_header),
          "m" (success)
    );
    return success;
}