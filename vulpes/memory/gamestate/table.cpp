/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

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
