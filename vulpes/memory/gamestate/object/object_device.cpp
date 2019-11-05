/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <hooker/hooker.hpp>

#include "object_device.hpp"

GenericTable* device_groups_table(){
    static GenericTable** effect_ptrs_loc = effect_ptrs();
    return effect_ptrs_loc[8];
}
