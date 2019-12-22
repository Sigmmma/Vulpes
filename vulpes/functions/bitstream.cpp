/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <vulpes/memory/signatures.hpp>

#include "bitstream.hpp"

extern "C" {
    uintptr_t func_bitstream_write_ptr;
    uintptr_t func_bitstream_read_ptr;
}
__attribute__((naked))
void bitstream_write(void* const iteration_header,
                 uint32_t* const write_this,
                         int32_t num_bits) {
    asm (
        "pushad;"
        "mov ecx, [esp+0x20+4];"
        "lea eax, [esp+0x20+8];"
        "mov edx, [esp+0x20+0xC];"
        "push edx;"
        "call [_func_bitstream_write_ptr];"
        "add esp, 4;"
        "popad;"
        "ret;"
    );
}

__attribute__((naked))
void bitstream_read(void* const iteration_header,
                uint32_t* const write_here,
                        int32_t num_bits) {
    asm (
        "pushad;"
        "mov edx, [esp+0x20+4];"
        "lea ecx, [esp+0x20+8];"
        "mov eax, [esp+0x20+0xC];"
        "push edx;"
        "call [_func_bitstream_read_ptr];"
        "add esp, 4;"
        "popad;"
        "ret;"
    );
}

void init_bitstream() {
    func_bitstream_write_ptr = sig_func_bitstream_write();
    func_bitstream_read_ptr  = sig_func_bitstream_read();
}
