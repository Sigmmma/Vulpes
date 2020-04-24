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

#pragma once

#include <vulpes/memory/tag/tag_types.hpp>

#include <vulpes/memory/network/message_delta.hpp>
#include <vulpes/memory/types.hpp>


// These are used to determine how to handle certain events for
// objects and other gamestate things.

// Typedeffing these so that we have central definitions.
// The first arg is always the raw object MemRef

typedef void (*ObjAdjustPlacement)(uint32_t,uint32_t);
typedef  bool(*ObjCreate)(uint32_t);
typedef void (*ObjPlace)(uint32_t,uint32_t);
typedef void (*ObjDestroy)(uint32_t);
typedef  bool(*ObjUpdate)(uint32_t);
typedef void (*ObjExportFunctionValues)(uint32_t);
typedef void (*ObjHandleDeletedObject)(uint32_t,uint32_t);
typedef void (*ObjHandleRegionDestroyed)(uint32_t,uint32_t,uint32_t);
typedef  bool(*ObjHandleParentDestroyed)(uint32_t);
typedef void (*ObjPreprocessNodeOrientations)(uint32_t,uint32_t);
typedef void (*ObjPostprocessNodeMatricies)(uint32_t,uint32_t);
typedef void (*ObjReset)(uint32_t);
typedef void (*ObjDisconnectFromStructureBsp)(uint32_t);
typedef void (*ObjNotifyImpulseSound)(uint32_t,uint32_t,uint32_t);
typedef void (*ObjRenderDebug)(uint32_t);
// Gbx
typedef void (*ObjNewToNetwork)(uint32_t,uint32_t,uint32_t,uint32_t);
typedef  bool(*ObjUpdateBaseline)(uint32_t);
typedef void (*ObjGenerateUpdateHeader)(uint32_t,void* /*output_buffer*/,uint32_t /* edx for mdp_encode*/,uint32_t);
typedef  bool(*ObjHandleReceivedUpdate)(uint32_t,uint32_t,int16_t,uint32_t*);
typedef  bool(*ObjShouldDoUpdateIncremental)(uint32_t);
typedef void (*ObjSetLastUpdateTime)(uint32_t);

//

struct ObjectBehaviorDefinition {
    char* type_name;
    TagTypes type_id;
    int32_t unknown;
    int32_t unknown2;
    MessageDeltaType network_update_header;
    PAD(3);
    // Nullsubs.
    void (*initialize)();
    void (*dispose)();
    void (*initialize_for_new_map)();
    void (*dispose_from_old_map)();
    // Actual per object update functions.
    ObjAdjustPlacement            adjust_placement;
    ObjCreate                     create;
    ObjPlace                      place;
    ObjDestroy                    destroy;
    ObjUpdate                     update; // Return true
    ObjExportFunctionValues       export_function_values;
    ObjHandleDeletedObject        handle_deleted_object;
    ObjHandleRegionDestroyed      handle_region_destroyed;
    ObjHandleParentDestroyed      handle_parent_destroyed;
    ObjPreprocessNodeOrientations preprocess_node_orientations;
    ObjPostprocessNodeMatricies   postprocess_node_matricies;
    ObjReset                      reset;
    ObjDisconnectFromStructureBsp disconnect_from_structure_bsp;
    ObjNotifyImpulseSound         notify_impulse_sound;
    ObjRenderDebug                render_debug; // Just a nullsub in release builds
    ////// Gearbox Additions - All network related. //////
    // Just returns false
    bool (*nullsub_ret_false)(uint32_t);
    // May also have a direct reference
    ObjNewToNetwork               new_to_network;
    // Copies values from the base object to the update_baseline in the extension for that object type
    ObjUpdateBaseline             update_baseline;
    // Returns false
    ObjGenerateUpdateHeader       generate_update_header;
    // Deals with receiving packets. Half of the inputs are in registers.
    ObjHandleReceivedUpdate       handle_received_update;
    // Returns true if it is time for an incremental update.
    ObjShouldDoUpdateIncremental  should_do_update_incremental;
    // Just returns true
    bool (*nullsub_ret_true)(uint32_t);
    // Returns true if it is time for a baseline update.
    ObjSetLastUpdateTime          set_last_update_time;
    // Back to Bungie
    ObjectBehaviorDefinition* parent_definitions[18];
};

ObjectBehaviorDefinition** object_behavior_defs();
