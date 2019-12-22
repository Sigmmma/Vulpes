/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <cstdint>

#include <vulpes/memory/signatures.hpp>

#include "table.hpp"

uint32_t datum_new(void* data) {
    uintptr_t func_datum_new_ptr = sig_func_datum_new();
    uint32_t return_value;
    asm (
        "pushad;"
        "mov edx, %1;"
        "call %0;"
        "mov %2, eax;"
        "popad;"
        :
        : "m" (func_datum_new_ptr), "m" (data), "m" (return_value)
    );
    return return_value;
}

uint32_t datum_new_at_index(void* data, uint32_t id) {
    uintptr_t func_datum_new_at_index_ptr = sig_func_datum_new_at_index();
    uint32_t return_value;
    asm (
        "pushad;"
        "mov edx, %1;"
        "mov eax, %3;"
        "call %0;"
        "mov %2, eax;"
        "popad;"
        :
        : "m" (func_datum_new_at_index_ptr), "m" (data), "m" (return_value), "m" (id)
    );
    return return_value;
}

uint32_t datum_delete(void* data, uint32_t id) {
    uintptr_t func_datum_delete_ptr = sig_func_datum_delete();
    uint32_t return_value;
    asm (
        "pushad;"
        "mov eax, %1;"
        "mov edx, %3;"
        "call %0;"
        "mov %2, eax;"
        "popad;"
        :
        : "m" (func_datum_delete_ptr), "m" (data), "m" (return_value), "m" (id)
    );
    return return_value;
}
