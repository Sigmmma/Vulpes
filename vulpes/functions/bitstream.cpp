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

#include <vulpes/memory/signatures/signatures.hpp>

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
