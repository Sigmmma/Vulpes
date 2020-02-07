/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <vulpes/memory/signatures.hpp>

#include "object.hpp"

ObjectTable* object_table() {
    return *reinterpret_cast<ObjectTable**>(
        sig_object_table_ref());;
}
