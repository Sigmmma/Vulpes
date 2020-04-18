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

#include <cstring>

#include <vulpes/functions/table.hpp>

#include "table.hpp"

MemRef Table::allocate() {
    MemRef return_value;
    return_value.raw = datum_new(this);
    return return_value;
}
/*
MemRef Table::new_at(MemRef id) {
    MemRef return_value;
    return_value.raw = datum_new_at_index(this, id.raw);
    return return_value;
}
*/
bool Table::remove(MemRef id) {
    return (datum_delete(this, id.raw) != 0xFFFFFFFF);
}

size_t Table::count() {
    // The reason we wrote it like this is to make it universal.
    // So it does not need to be overwritten for every table type.
    size_t count = 0;

    auto table = reinterpret_cast<GenericTable*>(this);
    // Table array starts here.
    uint32_t array_start_address = reinterpret_cast<uint32_t>(table->first);

    // Count all the valid entries.
    for (int i = 0; i < table->max_elements; i++) {
        uint32_t offset = table->element_size * i;

        // The first two bytes of a table entry is always the salt.
        uint16_t salt = *reinterpret_cast<uint16_t*>(array_start_address + offset);

        // If the salt is non-zero that means the entry is occupied.
        if (salt) count++;
    }

    return count;
}

void GenericTable::init(const char* name, uint16_t e_max, uint16_t e_size, void* array) {
    // Do the same setup for the table header that Halo does.
    memset(this, 0, sizeof(this));
    strncpy(this->name, name, sizeof(this->name) - 1);

    this->max_elements = e_max;
    this->element_size = e_size;

    // Just hex for the NOT terminated string 'd@t@'
    // (Vanilla Halo has this here, I don't think it actually serves a purpose.)
    this->sig = 0x64407440; //'d@t@'

    // Mark as invalid because vanilla Halo does that for some reason.
    this->is_valid = false;

    this->first = array;

    // Null the whole array.
    memset(this->first, 0, this->max_elements * this->element_size);
}

void GenericTable::init(const char* name, uint16_t e_max, uint16_t e_size, uintptr_t array) {
    this->init(name, e_max, e_size, reinterpret_cast<void*>(array));
}
