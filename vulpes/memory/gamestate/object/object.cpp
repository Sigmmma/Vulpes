/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <vulpes/memory/signatures.hpp>

#include "object.hpp"

ObjectTable* object_table() {
    static ObjectTable** object_table_ptr;
    if (!object_table_ptr) {
        object_table_ptr = reinterpret_cast<ObjectTable**>(
            sig_object_table_ref());
    }
    return *object_table_ptr;
}
