/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#pragma once
#include "types.hpp"

// These are used to determine how to handle certain events for
// objects and other gamestate things.

struct ObjectDataDefinition {
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
    void (*adjust_placement)(MemRef,uint32_t);
    void (*new)(MemRef);
    void (*place)(MemRef,uint32_t);
    void (*delete)(MemRef);
    void (*update)(MemRef); // Return true
    void (*export_function_values)(MemRef);
    void (*handle_deleted_object)(MemRef,uint32_t);
    void (*handle_region_destroyed)(MemRef,uint32_t,uint32_t);
    void (*handle_parent_destroyed)(MemRef);
    void (*preprocess_node_orientations)(MemRef,uint32_t);
    void (*postprocess_node_matricies)(MemRef,uint32_t);
    void (*reset)(MemRef);
    void (*disconnect_from_structure_bsp)(MemRef);
    void (*notify_impulse_sound)(MemRef,uint32_t,uint32_t);
    void (*render_debug)(MemRef); // Just a nullsub in release builds
    ////// Gearbox Additions - All network related. //////
    // Just returns false
    bool (*nullsub_ret_false)(MemRef);
    // May also have a direct reference
    void (*new_to_network)(MemRef,uint32_t,uint32_t,uint32_t);
    // Copies values from the base object to the update_baseline in the extension for that object type
    bool (*update_baseline)(MemRef);
    // Returns false
    void (*generate_update_header)(MemRef,void* /*output_buffer*/,uint32_t /* edx for mdp_encode*/,uint32_t);
    // Deals with receiving packets. Half of the inputs are in registers.
    void (*handle_received_update)(MemRef,uint32_t,int16_t,uint32_t*);
    void (*should_do_update_incremental)(MemRef);
    // Just returns true
    bool (*nullsub_ret_true)(MemRef);
    void (*should_do_update_baseline)(MemRef);
    // Back to Bungie
    DataDefinition* parent_definitions[16];
};
