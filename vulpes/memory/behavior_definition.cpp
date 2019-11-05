/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <hooker/hooker.hpp>

#include "behavior_definition.hpp"

Signature(true, sig_object_behavior_defs,
    {0x8B, 0x0C, 0x95, -1, -1, -1, 0x00,
     0x39, 0x71, 0x10, 0x74, 0x02, 0x33, 0xC0});

ObjectBehaviorDefinition** object_behavior_defs() {
    static ObjectBehaviorDefinition*** object_behave_ptr =
        reinterpret_cast<ObjectBehaviorDefinition***>(sig_object_behavior_defs.address()+3);
    return *object_behave_ptr;
}
