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

#include <cstdint>

#include <vulpes/memory/signatures/signatures.hpp>

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
