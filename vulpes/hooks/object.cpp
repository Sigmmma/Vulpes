/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <cstring>
#include <windows.h>

#include <hooker/function_pointer_safe.hpp>
#include <hooker/hooker.hpp>

#include <vulpes/memory/behavior_definition.hpp>
#include <vulpes/memory/gamestate/object/object.hpp>
#include <vulpes/memory/types.hpp>
#include <vulpes/memory/signatures.hpp>


// Signatures for finding stuff.
// Currently unused. Will be used soon.

/* Unused
Signature(true, sig_object_update,
    {0x51, 0x8B, 0x0D, -1, -1, -1, 0x00, 0x8B, 0x51, 0x34, 0x53, 0x8B,
     0x5C, 0x24, 0x0C, 0x8B, 0xC3, 0x25, 0xFF, 0xFF, 0x00, 0x00}); // + 0 size 7

Signature(true, sig_objects_update,
    {0x00, 0x8A, -1, 0x0C, 0x83, 0xEC, 0x0C}); // -5 on client, -4 on server. size 6 on client, size 5 on server.
*/

// Weapon signatures

/* Unused for now.
Signature(true, sig_weapon_fire_call,
    {0x8B, 0x8C, 0x24, 0xAC, 0x00, 0x00, 0x00, 0x50, 0x51, 0x56,
     0xE8, -1, -1, 0x00, 0x00, 0x83, 0xC4, 0x0C}); // + 0xA size 5
Signature(true, sig_weapon_fire_object_new_call,
    {0x8D, 0x84, 0x24, 0xA4, 0x00, 0x00, 0x00, 0x50,
     0xE8, -1, -1, -1, 0x00, 0x8B, 0xF0}); // + 8 size 5
*/

// New behavior definitions, when initialized hooking into
// one of these functions is as simple as re-assigning
// the pointer to a replacement function.


ObjectBehaviorDefinition new_obje_behavior_def;
ObjectBehaviorDefinition new_devi_behavior_def;
ObjectBehaviorDefinition new_item_behavior_def;
ObjectBehaviorDefinition new_unit_behavior_def;

ObjectBehaviorDefinition new_bipd_behavior_def;
ObjectBehaviorDefinition new_vehi_behavior_def;
ObjectBehaviorDefinition new_weap_behavior_def;

ObjectBehaviorDefinition new_eqip_behavior_def;
ObjectBehaviorDefinition new_garb_behavior_def;
ObjectBehaviorDefinition new_proj_behavior_def;

ObjectBehaviorDefinition new_scen_behavior_def;
ObjectBehaviorDefinition new_mach_behavior_def;
ObjectBehaviorDefinition new_ctrl_behavior_def;
ObjectBehaviorDefinition new_lifi_behavior_def;

ObjectBehaviorDefinition new_plac_behavior_def;
ObjectBehaviorDefinition new_ssce_behavior_def;


extern "C" { // Demangle naming for ASM.

    //////////
    // Declaring all function pointers for each object type here
    // so that the assembly can access it easily if needed.

    // Defining this as a macro since it is repeated 16 times.

    #define DELC_OBJ_BEHAVIOR_FUNCTION_PTRS(type) \
        ObjAdjustPlacement            type ## _adjust_placement              = NULL; \
        ObjCreate                     type ## _create                        = NULL; \
        ObjPlace                      type ## _place                         = NULL; \
        ObjDestroy                    type ## _destroy                       = NULL; \
        ObjUpdate                     type ## _update                        = NULL; \
        ObjExportFunctionValues       type ## _export_function_values        = NULL; \
        ObjHandleDeletedObject        type ## _handle_deleted_object         = NULL; \
        ObjHandleRegionDestroyed      type ## _handle_region_destroyed       = NULL; \
        ObjHandleParentDestroyed      type ## _handle_parent_destroyed       = NULL; \
        ObjPreprocessNodeOrientations type ## _preprocess_node_orientations  = NULL; \
        ObjPostprocessNodeMatricies   type ## _postprocess_node_matricies    = NULL; \
        ObjReset                      type ## _reset                         = NULL; \
        ObjDisconnectFromStructureBsp type ## _disconnect_from_structure_bsp = NULL; \
        ObjNotifyImpulseSound         type ## _notify_impulse_sound          = NULL; \
        ObjRenderDebug                type ## _render_debug                  = NULL; \
        ObjNewToNetwork               type ## _new_to_network                = NULL; \
        ObjUpdateBaseline             type ## _update_baseline               = NULL; \
        ObjGenerateUpdateHeader       type ## _generate_update_header        = NULL; \
        ObjHandleReceivedUpdate       type ## _handle_received_update        = NULL; \
        ObjShouldDoUpdateIncremental  type ## _should_do_update_incremental  = NULL; \
        ObjSetLastUpdateTime          type ## _set_last_update_time          = NULL; \

    // Negative type indices

    DELC_OBJ_BEHAVIOR_FUNCTION_PTRS(object);
    DELC_OBJ_BEHAVIOR_FUNCTION_PTRS(device);
    DELC_OBJ_BEHAVIOR_FUNCTION_PTRS(item);
    DELC_OBJ_BEHAVIOR_FUNCTION_PTRS(unit);

    // Positive (Valid) type indices

    DELC_OBJ_BEHAVIOR_FUNCTION_PTRS(biped);
    DELC_OBJ_BEHAVIOR_FUNCTION_PTRS(vehicle);
    DELC_OBJ_BEHAVIOR_FUNCTION_PTRS(weapon);

    DELC_OBJ_BEHAVIOR_FUNCTION_PTRS(equipment);
    DELC_OBJ_BEHAVIOR_FUNCTION_PTRS(garbage);
    DELC_OBJ_BEHAVIOR_FUNCTION_PTRS(projectile);

    DELC_OBJ_BEHAVIOR_FUNCTION_PTRS(scenery);

    DELC_OBJ_BEHAVIOR_FUNCTION_PTRS(machine);
    DELC_OBJ_BEHAVIOR_FUNCTION_PTRS(control);
    DELC_OBJ_BEHAVIOR_FUNCTION_PTRS(light_fixture);

    DELC_OBJ_BEHAVIOR_FUNCTION_PTRS(placeholder);
    DELC_OBJ_BEHAVIOR_FUNCTION_PTRS(sound_scenery);

    #undef DELC_OBJ_BEHAVIOR_FUNCTION_PTRS

}

static ObjectBehaviorDefinition* vanilla_def_pointers_backup[POSITIVE_OBJECT_TYPES];

extern "C" {

    uintptr_t after_object_hook__create;
    extern object_create_wrapper();

    uintptr_t after_weapon_hook__pull_trigger;
    extern weapon_pull_trigger_wrapper();

    uintptr_t after_biped_hook__jump;
    extern biped_jump_wrapper();

}

struct ObjectCreateArgs {
    int unknown;
    int unknown2;
};

extern "C" bool before_object_create(ObjectCreateArgs* args) {

    // Stub: Hook current unused.

    return true;
}

extern "C" void after_object_create(uint32_t* obj) {

    // Stub: Hook current unused.

}

Patch(
    object_create_hook_patch, 0, 6,
    JMP_PATCH, &object_create_wrapper
);

// Only called when a biped actually jumps

extern "C" bool before_biped_jump(uint32_t* obj) {

    // Stub: Hook currently unused.

    return true;
}

extern "C" void after_biped_jump(uint32_t* obj) {

    // Stub: Hook currently unused.

}

Patch(
    biped_jump_hook_patch, 0, 6,
    JMP_PATCH, &biped_jump_wrapper
);

struct WeaponPullTriggerArgs {
    uint32_t obj;
    int trigger_id;
};

extern "C" bool before_weapon_pull_trigger(WeaponPullTriggerArgs* args) {

    // Stub: Hook current unused.

    return true;
}

extern "C" void after_weapon_pull_trigger() {

    // Stub: Hook current unused.

}

Patch(
    weapon_pull_trigger_hook_patch, 0, 6,
    JMP_PATCH, &weapon_pull_trigger_wrapper
);


void init_object_hooks() {
    auto game_defs = object_behavior_defs();

    // Copy data to our new defs.

    size_t def_size = sizeof(ObjectBehaviorDefinition);

    // These top 4 are negative indices and are not in the base array,
    // but we still need them. They are parent types of other object types
    // found here though, so we get that data by looking through the parent
    // type definitions.

    memcpy(
        &new_obje_behavior_def,
        game_defs[etoi(ObjectType::BIPED)]->parent_definitions[0],
        def_size);
    new_obje_behavior_def.parent_definitions[0] = &new_obje_behavior_def;

    memcpy(
        &new_devi_behavior_def,
        game_defs[etoi(ObjectType::MACHINE)]->parent_definitions[1],
        def_size);
    new_devi_behavior_def.parent_definitions[0] = &new_obje_behavior_def;
    new_devi_behavior_def.parent_definitions[1] = &new_devi_behavior_def;

    memcpy(
        &new_item_behavior_def,
        game_defs[etoi(ObjectType::WEAPON)]->parent_definitions[1],
        def_size);
    new_item_behavior_def.parent_definitions[0] = &new_obje_behavior_def;
    new_item_behavior_def.parent_definitions[1] = &new_item_behavior_def;

    memcpy(
        &new_unit_behavior_def,
        game_defs[etoi(ObjectType::BIPED)]->parent_definitions[1],
        def_size);
    new_unit_behavior_def.parent_definitions[0] = &new_obje_behavior_def;
    new_unit_behavior_def.parent_definitions[1] = &new_unit_behavior_def;

    // 0 and up

    memcpy(&new_bipd_behavior_def,
        game_defs[etoi(ObjectType::BIPED)],
        def_size);
    new_bipd_behavior_def.parent_definitions[0] = &new_obje_behavior_def;
    new_bipd_behavior_def.parent_definitions[1] = &new_unit_behavior_def;
    new_bipd_behavior_def.parent_definitions[2] = &new_bipd_behavior_def;

    memcpy(&new_vehi_behavior_def,
        game_defs[etoi(ObjectType::VEHICLE)],
        def_size);
    new_vehi_behavior_def.parent_definitions[0] = &new_obje_behavior_def;
    new_vehi_behavior_def.parent_definitions[1] = &new_unit_behavior_def;
    new_vehi_behavior_def.parent_definitions[2] = &new_vehi_behavior_def;

    memcpy(
        &new_weap_behavior_def,
        game_defs[etoi(ObjectType::WEAPON)],
        def_size);
    new_weap_behavior_def.parent_definitions[0] = &new_obje_behavior_def;
    new_weap_behavior_def.parent_definitions[1] = &new_item_behavior_def;
    new_weap_behavior_def.parent_definitions[2] = &new_weap_behavior_def;

    memcpy(
        &new_eqip_behavior_def,
        game_defs[etoi(ObjectType::EQUIPMENT)],
        def_size);
    new_eqip_behavior_def.parent_definitions[0] = &new_obje_behavior_def;
    new_eqip_behavior_def.parent_definitions[1] = &new_item_behavior_def;
    new_eqip_behavior_def.parent_definitions[2] = &new_eqip_behavior_def;

    memcpy(&new_garb_behavior_def,
        game_defs[etoi(ObjectType::GARBAGE)],
        def_size);
    new_garb_behavior_def.parent_definitions[0] = &new_obje_behavior_def;
    new_garb_behavior_def.parent_definitions[1] = &new_item_behavior_def;
    new_garb_behavior_def.parent_definitions[2] = &new_garb_behavior_def;

    memcpy(&new_proj_behavior_def,
        game_defs[etoi(ObjectType::PROJECTILE)],
        def_size);
    new_proj_behavior_def.parent_definitions[0] = &new_obje_behavior_def;
    new_proj_behavior_def.parent_definitions[1] = &new_proj_behavior_def;

    memcpy(&new_scen_behavior_def,
        game_defs[etoi(ObjectType::SCENERY)],
        def_size);
    new_scen_behavior_def.parent_definitions[0] = &new_obje_behavior_def;
    new_scen_behavior_def.parent_definitions[1] = &new_scen_behavior_def;

    memcpy(&new_mach_behavior_def,
        game_defs[etoi(ObjectType::MACHINE)],
        def_size);
    new_mach_behavior_def.parent_definitions[0] = &new_obje_behavior_def;
    new_mach_behavior_def.parent_definitions[1] = &new_devi_behavior_def;
    new_mach_behavior_def.parent_definitions[2] = &new_mach_behavior_def;

    memcpy(&new_ctrl_behavior_def,
        game_defs[etoi(ObjectType::CONTROL)],
        def_size);
    new_ctrl_behavior_def.parent_definitions[0] = &new_obje_behavior_def;
    new_ctrl_behavior_def.parent_definitions[1] = &new_devi_behavior_def;
    new_ctrl_behavior_def.parent_definitions[2] = &new_ctrl_behavior_def;

    memcpy(&new_lifi_behavior_def,
        game_defs[etoi(ObjectType::LIGHT_FIXTURE)],
        def_size);
    new_lifi_behavior_def.parent_definitions[0] = &new_obje_behavior_def;
    new_lifi_behavior_def.parent_definitions[1] = &new_devi_behavior_def;
    new_lifi_behavior_def.parent_definitions[2] = &new_lifi_behavior_def;

    memcpy(&new_plac_behavior_def,
        game_defs[etoi(ObjectType::PLACEHOLDER)],
        def_size);
    new_plac_behavior_def.parent_definitions[0] = &new_obje_behavior_def;
    new_plac_behavior_def.parent_definitions[1] = &new_plac_behavior_def;

    memcpy(&new_ssce_behavior_def,
        game_defs[etoi(ObjectType::SOUND_SCENERY)],
        def_size);
    new_ssce_behavior_def.parent_definitions[0] = &new_obje_behavior_def;
    new_ssce_behavior_def.parent_definitions[1] = &new_ssce_behavior_def;

    // Backup old array of def pointers for when the DLL needs to unload.

    size_t array_size = sizeof(uintptr_t)*POSITIVE_OBJECT_TYPES;

    memcpy(&vanilla_def_pointers_backup, game_defs, array_size);

    // Replace old array of def pointers with pointers to our new defs.

    DWORD prota, protb;
    VirtualProtect(game_defs, array_size, PAGE_EXECUTE_READWRITE, &prota);

    game_defs[etoi(ObjectType::BIPED)]          = &new_bipd_behavior_def;
    game_defs[etoi(ObjectType::VEHICLE)]        = &new_vehi_behavior_def;
    game_defs[etoi(ObjectType::WEAPON)]         = &new_weap_behavior_def;
    game_defs[etoi(ObjectType::EQUIPMENT)]      = &new_eqip_behavior_def;
    game_defs[etoi(ObjectType::GARBAGE)]        = &new_garb_behavior_def;
    game_defs[etoi(ObjectType::PROJECTILE)]     = &new_proj_behavior_def;
    game_defs[etoi(ObjectType::SCENERY)]        = &new_scen_behavior_def;
    game_defs[etoi(ObjectType::MACHINE)]        = &new_mach_behavior_def;
    game_defs[etoi(ObjectType::CONTROL)]        = &new_ctrl_behavior_def;
    game_defs[etoi(ObjectType::LIGHT_FIXTURE)]  = &new_lifi_behavior_def;
    game_defs[etoi(ObjectType::PLACEHOLDER)]    = &new_plac_behavior_def;
    game_defs[etoi(ObjectType::SOUND_SCENERY)]  = &new_ssce_behavior_def;

    VirtualProtect(game_defs, array_size, prota, &protb);

    // Copy the function pointers to our assembly callable vars
    // before we hook into them.
    // We're going to have to manually copy all of these, so we use another macro.

    #define COPY_EVENT_DEF(type, def) \
        type ## _adjust_placement = def.adjust_placement; \
        type ## _create = def.create; \
        type ## _place = def.place; \
        type ## _destroy = def.destroy; \
        type ## _update = def.update; \
        type ## _export_function_values = def.export_function_values; \
        type ## _handle_deleted_object = def.handle_deleted_object; \
        type ## _handle_region_destroyed = def.handle_region_destroyed; \
        type ## _handle_parent_destroyed = def.handle_parent_destroyed; \
        type ## _preprocess_node_orientations = def.preprocess_node_orientations; \
        type ## _postprocess_node_matricies = def.postprocess_node_matricies; \
        type ## _reset = def.reset; \
        type ## _disconnect_from_structure_bsp = def.disconnect_from_structure_bsp; \
        type ## _notify_impulse_sound = def.notify_impulse_sound; \
        type ## _render_debug = def.render_debug; \
        type ## _new_to_network = def.new_to_network; \
        type ## _update_baseline = def.update_baseline; \
        type ## _generate_update_header = def.generate_update_header; \
        type ## _handle_received_update = def.handle_received_update; \
        type ## _should_do_update_incremental = def.should_do_update_incremental; \
        type ## _set_last_update_time = def.set_last_update_time; \
        // End of Macro

    COPY_EVENT_DEF(object,          new_obje_behavior_def);
    COPY_EVENT_DEF(device,          new_devi_behavior_def);
    COPY_EVENT_DEF(item,            new_item_behavior_def);
    COPY_EVENT_DEF(unit,            new_unit_behavior_def);

    COPY_EVENT_DEF(biped,           new_bipd_behavior_def);
    COPY_EVENT_DEF(vehicle,         new_vehi_behavior_def);
    COPY_EVENT_DEF(weapon,          new_weap_behavior_def);
    COPY_EVENT_DEF(equipment,       new_eqip_behavior_def);
    COPY_EVENT_DEF(garbage,         new_garb_behavior_def);
    COPY_EVENT_DEF(projectile,      new_proj_behavior_def);
    COPY_EVENT_DEF(scenery,         new_scen_behavior_def);
    COPY_EVENT_DEF(machine,         new_mach_behavior_def);
    COPY_EVENT_DEF(control,         new_ctrl_behavior_def);
    COPY_EVENT_DEF(light_fixture,   new_lifi_behavior_def);
    COPY_EVENT_DEF(placeholder,     new_plac_behavior_def);
    COPY_EVENT_DEF(sound_scenery,   new_ssce_behavior_def);

    #undef COPY_EVENT_DEF

    // Put hooks in place.
    // Hooking into one of the above copied functions is as easy as replacing
    // its pointer with the pointer to the replacing function.
    // In most cases we will still need to execute the old one though. So,
    // when doing that make sure to call the old function. It can simply be
    // called by name because of how we've copied them.

    //////// Other hooks:

    // Objects:

    object_create_hook_patch.build(sig_hook_object_create());
    object_create_hook_patch.apply();
    after_object_hook__create =
        object_create_hook_patch.return_address();


    // Bipeds:

    biped_jump_hook_patch.build(sig_hook_biped_jump());
    biped_jump_hook_patch.apply();
    after_biped_hook__jump =
        biped_jump_hook_patch.return_address();


    // Weapons:

    weapon_pull_trigger_hook_patch.build(sig_hook_weapon_pull_trigger());
    weapon_pull_trigger_hook_patch.apply();
    after_weapon_hook__pull_trigger =
        weapon_pull_trigger_hook_patch.return_address();


}

void revert_object_hooks() {

    auto game_defs = object_behavior_defs();

    // Revert to the old defs.

    size_t array_size = sizeof(uintptr_t)*POSITIVE_OBJECT_TYPES;

    DWORD prota, protb;
    VirtualProtect(game_defs, array_size, PAGE_EXECUTE_READWRITE, &prota);
    memcpy(game_defs, &vanilla_def_pointers_backup, array_size);
    VirtualProtect(game_defs, array_size, prota, &protb);

    //////// Other hooks:

    // Weapons:

    weapon_pull_trigger_hook_patch.revert();

}
