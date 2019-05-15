#include "bitstream.hpp"
#include "../../hooks/hooker.hpp"

Signature(true, sig_bitstream_write,
    { 0x53, 0x55, 0x8B, 0x6C, 0x24, 0x0C, 0x85, 0xED, 0x56, 0x57, 0x8B,
      0xF0, 0x8B, 0xD9, 0x8B, 0xFD, 0x7E });
Signature(true, sig_bitstream_read,
    { 0x53, 0x55, 0x8B, 0x6C, 0x24, 0x0C, 0x56, 0x8B, 0xF0, 0x33, 0xDB,
      0x85, 0xF6, 0x57, 0x8B, 0xF9, 0x7E });

static uintptr_t func_bitstream_write_ptr;
static uintptr_t func_bitstream_read_ptr;

__attribute__((naked))
void bitstream_write(void* const iteration_header,
                 uint32_t* const write_this,
                         int32_t num_bits){
    asm (
        "pushad;"
        "mov ecx, [esp+0x20+4];"
        "lea eax, [esp+0x20+8];"
        "mov edx, [esp+0x20+0xC];"
        "push edx;"
        "call %0;"
        "add esp, 4;"
        "popad;"
        "ret;"
        : "+m" (func_bitstream_write_ptr)
    );
}

__attribute__((naked))
void bitstream_read(void* const iteration_header,
                uint32_t* const write_here,
                        int32_t num_bits){
    asm (
        "pushad;"
        "mov edx, [esp+0x20+4];"
        "mov ecx, [esp+0x20+8];"
        "mov eax, [esp+0x20+0xC];"
        "push edx;"
        "call %0;"
        "add esp, 4;"
        "popad;"
        "ret;"
        : "+m" (func_bitstream_read_ptr)
    );
}

void init_bitstream(){
    func_bitstream_write_ptr = sig_bitstream_write.get_address();
    func_bitstream_read_ptr  = sig_bitstream_read.get_address();
}
