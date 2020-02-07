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
