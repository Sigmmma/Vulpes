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
    size_t count = 0;
    auto table = reinterpret_cast<GenericTable*>(this);
    for (int i = 0; i < max_elements; i++) {
        int32_t offset = element_size * i;
        int32_t base = reinterpret_cast<int32_t>(table->first);
        int16_t salt = *reinterpret_cast<int16_t*>(base + offset);
        if (salt) count++;
    }

    return count;
}
