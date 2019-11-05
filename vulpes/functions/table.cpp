/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <cstdint>

#include <hooker/hooker.hpp>

#include "table.hpp"

Signature(true, sig_datum_new,
    {0x0F, 0xBF, 0x4A, 0x22, 0x53, 0x66, 0x8B, 0x5A, 0x2C, 0x55});
Signature(true, sig_datum_new_at_index,
    {0x57, 0x8B, 0xF8, 0xC1, 0xFF, 0x10, 0x66, 0x85, 0xC0, 0x7C});
Signature(true, sig_datum_delete,
    {0x56, 0x8B, 0xF2, 0xC1, 0xFE, 0x10, 0x66, 0x85, 0xD2, 0x57});

/*
data_iterator_next(<edi> TableIterator)
66 8B 4F 04 53 55 56 8B 37 0F BF 6E 22
*/

uint32_t datum_new(void* data) {
    static intptr_t func_datum_new = sig_datum_new.address();
    uint32_t return_value;
    asm (
        "pushad;"
        "mov edx, %1;"
        "call %0;"
        "mov %2, eax;"
        "popad;"
        :
        : "m" (func_datum_new), "m" (data), "m" (return_value)
    );
    return return_value;
}

uint32_t datum_new_at_index(void* data, uint32_t id) {
    static intptr_t func_datum_new_at_index = sig_datum_new_at_index.address();
    uint32_t return_value;
    asm (
        "pushad;"
        "mov edx, %1;"
        "mov eax, %3;"
        "call %0;"
        "mov %2, eax;"
        "popad;"
        :
        : "m" (func_datum_new_at_index), "m" (data), "m" (return_value), "m" (id)
    );
    return return_value;
}

uint32_t datum_delete(void* data, uint32_t id) {
    static intptr_t func_datum_delete = sig_datum_delete.address();
    uint32_t return_value;
    asm (
        "pushad;"
        "mov eax, %1;"
        "mov edx, %3;"
        "call %0;"
        "mov %2, eax;"
        "popad;"
        :
        : "m" (func_datum_delete), "m" (data), "m" (return_value), "m" (id)
    );
    return return_value;
}
