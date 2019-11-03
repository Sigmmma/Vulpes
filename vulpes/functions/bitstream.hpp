/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#pragma once
#include <cstdint>

// This is all depricated for now.

__attribute__((naked)) // ecx, eax, esp+4
void bitstream_write(void* const iteration_header,
                 uint32_t* const write_this,
                         int32_t num_bits);


__attribute__((naked)) // ecx, eax, esp+4
void bitstream_read(void* const iteration_header,
                uint32_t* const write_here,
                        int32_t num_bits);

void init_bitstream();
