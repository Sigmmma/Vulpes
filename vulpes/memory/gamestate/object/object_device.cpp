/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <vulpes/memory/signatures.hpp>

#include "object_device.hpp"

GenericTable* device_groups_table() {
    return reinterpret_cast<GenericTable**>(
        sig_effect_table_refs())[8];
}
