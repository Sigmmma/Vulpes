/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#pragma once

#include <vulpes/memory/types.hpp>

class Table {
public:

    // Checks if there is an empty entry and sets one up if there is,
    // returns a MemRef to the allocated id.
    MemRef allocate();
    // Deletes the entry at the given MemRef.
    // Returns true if succesful, false if it couldn't find it.
    bool remove(MemRef id);

    char name[0x20];
    int16_t max_count;
    int16_t index_size;
    bool valid;
    bool id_zero_valid;
    PAD(2);
    int32_t sig; // d@t@
    int16_t next_id;
    int16_t last_id;
    MemRef next;
}; static_assert(sizeof(Table) == 0x34);

class GenericTable : public Table {
public:
    void* first;
};

GenericTable** effect_ptrs();
