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

/*
    Halo allocates all of its gamestate important memory into a static buffer
    that is always the same size, and always in the same place.

    When we want to increase the amount of things we can store in gamestate
    the naive approach would be to extend that buffer. Problem is with that
    approach that there is other data after that buffer that is also always
    there. Opensauce also just extends this buffer, and we don't want to step
    on its toes.

    Our solution to this problem is to instead allocate another buffer right
    before this gamestate buffer. We try to fit as many things as possible into
    this buffer. We're excluding certain things though. Objects are still in
    the normal buffer because the old Chimera lua lets people directly access
    memory, and in sandboxed mode will not allow writes outside of that region.
    I don't want to deal with that incompatibility either.

    The problem with allocating another buffer in a different location is that
    Halo doesn't save this buffer's data to checkpoint files. Most of the hooks
    in this file hook into Halo's checkpoint saving and loading code so that
    our files are also saved, loaded, and copied at the right times.

    We also completely replace the table allocation function that the game
    normally uses because hooking into it more passively turned out to be a
    major pain in my ass. So, we have a function that allocates tables in both
    our and vanilla memory that does all the things Halo normally does.
*/

#include <cassert>
#include <cstdio>
#include <cstring>
#include <windows.h> /* VirtualAlloc, VirtualFree */

#include <hooker/hooker.hpp>
#include <util/crc32.hpp>
#include <vulpes/memory/persistent/global.hpp>
#include <vulpes/memory/signatures/signatures.hpp>
#include <vulpes/memory/table.hpp>

#include "gamestate.hpp"

// Vanilla

// base address for gamestate.
static uintptr_t* game_state_globals;
// Actually the end address for gamestate.
static uintptr_t* game_state_globals_cpu_allocation_size;
// The buffer in which a checkpoint to be saved resides.
static void** game_state_globals_autosave_thread;
// The amount of allocated memory in gamestate.
static const uintptr_t* game_state_globals_buffer_size;
// A sentinel value that Halo uses to avoid loading invalid saves.
static uintptr_t* game_state_globals_crc;
// The filehandle to the savefile.
/* The reason we don't use this is that Halo resizes the checkpoint file to
   a specific size every time Halo starts.
   We could fix this, but I'd rather avoid the potential compatibility conflict
   we could have with other mods over this.
*/
//static HANDLE*    game_state_globals_file_handle;

extern "C" { // These are shared with the assembly.

    // the jmp location for gamestate_copy_checkpoint_file
    uintptr_t gamestate_copy_checkpoint_file_continue_ptr;

    /* The original function that copies both the .sav and .bin files for a
       checkpoint to another location */
    __attribute__((regparm(2)))
    char gamestate_copy_checkpoint_file(char*, char*, char*);

    // These are assembly wrappers for functions in this file.

    void gamestate_table_new_wrapper();
    void gamestate_read_from_main_file_hook_wrapper();
    void gamestate_read_from_profile_file_hook_wrapper();
    void gamestate_write_to_files_hook_wrapper();
    char gamestate_copy_checkpoint_file_wrapper(char*, char*, char*);
}

// Upgrades

static uintptr_t used_extension_memory = 0;
static const size_t ALLOCATED_UPGRADE_MEMORY = 10*1024*1024; // 10 MB
static void* gamestate_extension_buffer;
static void* gamestate_extension_checkpoint_buffer;

// Misc constants
static const char* SAVE_PATH = "\\savegame.vulpes";
// Number of characters to allocate when building paths.
static const size_t PATH_CHARS = 4096;


struct TableUpgradeData {
    const char* name; // Name to apply this upgrade to.
    uint16_t new_max; // New max to use.
    bool in_upgrade_memory;
    // Whether or not to store this upgrade in upgrade memory.
    // Only really used for objects, so we don't break compat with the old
    // Chimera. As the sandboxed LUA in that version has a specific memory
    // range that it allows edits in.
};

const uint16_t TABLE_END_SENTINEL = 0;

const TableUpgradeData TABLE_UPGRADES[] = {
    {"object", TABLE_OBJECT_UPGR_MAX, false},
    {"cached object render states", TABLE_CACHED_OBJECT_RENDER_STATES_UPGR_MAX, true},
    {"widget", TABLE_WIDGET_UPGR_MAX, true},
    {"flag", TABLE_FLAG_UPGR_MAX, true},
    {"antenna", TABLE_ANTENNA_UPGR_MAX, true},
    {"glow", TABLE_GLOW_UPGR_MAX, true},
    {"glow particles", TABLE_GLOW_PARTICLES_UPGR_MAX, true},
    {"light volumes", TABLE_LIGHT_VOLUMES_UPGR_MAX, true},
    {"lightnings", TABLE_LIGHTNINGS_UPGR_MAX, true},
    {"device groups", TABLE_DEVICE_GROUPS_UPGR_MAX, true},
    {"lights", TABLE_LIGHTS_UPGR_MAX, true},
    {"cluster light reference", TABLE_CLUSTER_LIGHT_REFERENCE_UPGR_MAX, true},
    {"light cluster reference", TABLE_LIGHT_CLUSTER_REFERENCE_UPGR_MAX, true},
    {"cluster collideable object reference", TABLE_CLUSTER_COLLIDABLE_OBJECT_REFERENCE_UPGR_MAX, true},
    {"cluster noncollideable object reference", TABLE_CLUSTER_NONCOLLIDABLE_OBJECT_REFERENCE_UPGR_MAX, true},
    {"collideable object cluster reference", TABLE_COLLIDABLE_OBJECT_CLUSTER_REFERENCE_UPGR_MAX, true},
    {"noncollideable object cluster reference", TABLE_NONCOLLIDABLE_OBJECT_CLUSTER_REFERENCE_UPGR_MAX, true},
    {"decals", TABLE_DECALS_UPGR_MAX, true},
    {"players", TABLE_PLAYERS_UPGR_MAX, true},
    {"teams", TABLE_TEAMS_UPGR_MAX, true},
    {"contrail", TABLE_CONTRAIL_UPGR_MAX, true},
    {"contrail point", TABLE_CONTRAIL_POINT_UPGR_MAX, true},
    {"particle", TABLE_PARTICLE_UPGR_MAX, true},
    {"effect", TABLE_EFFECT_UPGR_MAX, true},
    {"effect location", TABLE_EFFECT_LOCATION_UPGR_MAX, true},
    {"particle systems", TABLE_PARTICLE_SYSTEMS_UPGR_MAX, true},
    {"particle system particles", TABLE_PARTICLE_SYSTEM_PARTICLES_UPGR_MAX, true},
    {"object looping sounds", TABLE_OBJECT_LOOPING_SOUNDS_UPGR_MAX, true},
    {"actor", TABLE_ACTOR_UPGR_MAX, true},
    {"swarm", TABLE_SWARM_UPGR_MAX, true},
    {"swarm component", TABLE_SWARM_COMPONENT_UPGR_MAX, true},
    {"prop", TABLE_PROP_UPGR_MAX, true},
    {"encounter", TABLE_ENCOUNTER_UPGR_MAX, true},
    {"ai pursuit", TABLE_AI_PERSUIT_UPGR_MAX, true},
    {"object list header", TABLE_OBJECT_LIST_HEADER_UPGR_MAX, false},
    {"list object reference", TABLE_LIST_OBJECT_REFERENCE_UPGR_MAX, false},
    {"hs thread", TABLE_HS_THREAD_UPGR_MAX, true},
    {"hs globals", TABLE_HS_GLOBALS_UPGR_MAX, true},
    {"recorded animations", TABLE_RECORDED_ANIMATIONS_UPGR_MAX, true},
    {"", TABLE_END_SENTINEL, false}, // Terminate
};

// Wonky argument order needed for the assembly wrapper to stay simple.
extern "C" __attribute__((regparm(1)))
GenericTable* gamestate_table_new_replacement(uint32_t element_size,
        const char* name, uint16_t element_max) {

    bool found = false;
    bool use_upgrade_memory = false;
    size_t i;
    for (i=0; !found && TABLE_UPGRADES[i].new_max != TABLE_END_SENTINEL; i++) {
        if (strncmp(name, TABLE_UPGRADES[i].name, 31) == 0) {
            found = true;
            break;
        }
    }

    if (found) {
        /* Use our limit if it is higher. Otherwise use the limit passed into
         * this function. Some other mod may have increased it.
         * And probably expects it to actually have that size.
         * We have extra memory allocated for this edge case. */
        element_max = (TABLE_UPGRADES[i].new_max > element_max) ?
                         TABLE_UPGRADES[i].new_max : element_max;
        use_upgrade_memory = TABLE_UPGRADES[i].in_upgrade_memory;
    }

    uintptr_t* mem_start;
    uintptr_t* mem_used;
    const uintptr_t* mem_max;

    if (use_upgrade_memory) {
        mem_start = reinterpret_cast<uintptr_t*>(&gamestate_extension_buffer);
        mem_used  = &used_extension_memory;
        mem_max   = &ALLOCATED_UPGRADE_MEMORY;
    } else {
        mem_start = game_state_globals;
        mem_used  = game_state_globals_cpu_allocation_size;
        mem_max   = game_state_globals_buffer_size;
    }

    // Get the next available spot in vanilla gamestate.
    auto new_table = reinterpret_cast<GenericTable*>(*mem_start + *mem_used);

    // Calculate and store the total used.
    *mem_used = *mem_used + sizeof(GenericTable) + element_size * element_max;

    // Make sure the limits are correctly enforced.
    if (*mem_used > *mem_max)
        printf("Table memory usage went over the amount of allocated memory.\n"
               "%d vs %d for table %s\n", *mem_used, *mem_max, name);
    assert(*mem_used <= *mem_max);

    new_table->init(
        name, element_max, element_size,
        reinterpret_cast<uintptr_t>(new_table) + sizeof(GenericTable)
    );

    // Halo does this as a security measure to avoid loading gamestates with
    // mismatched layouts.

    const int crc_buffer = element_max;
    *game_state_globals_crc = crc32(*game_state_globals_crc, &crc_buffer, sizeof(int));

    return new_table;
}

/* Reads upgraded gamestate from the given filepath */
static void game_state_upgrade_read_from_file(char* filepath) {
    FILE* save_file = fopen(filepath, "rb");
    // Read the upgrade memory from file.
    fread(gamestate_extension_buffer, 1, ALLOCATED_UPGRADE_MEMORY, save_file);

    // Close the file.
    fclose(save_file);
}

/* saves the upgraded gamestate to the given filepath */
static void game_state_upgrade_write_to_file(char* filepath) {
    FILE* save_file = fopen(filepath, "wb");
    // Just dump the entire upgrade memory into one file.
    fwrite(gamestate_extension_checkpoint_buffer, 1, ALLOCATED_UPGRADE_MEMORY, save_file);
    // Flush and close.
    fflush(save_file);
    fclose(save_file);
}

extern "C"
/* Loads the upgraded savegame from the main profile path */
void gamestate_read_from_main_file_hook() {
    char path[PATH_CHARS];

    // Find the path where the main savefile goes.

    strcpy(path, profile_path());
    strcat(path, SAVE_PATH);

    // Read directly from the main savefile extension

    game_state_upgrade_read_from_file(path);
}

extern "C"
/* Loads the upgraded savegame from the player profile folder */
void gamestate_read_from_profile_file_hook() {
    char path[PATH_CHARS];

    // Find the path where the main savefile goes.

    saved_game_file_get_path_to_enclosing_directory(active_profile_id(), path);
    strcat(path, SAVE_PATH);

    // Read directly from the main savefile extension

    game_state_upgrade_read_from_file(path);
}

extern "C"
/* Halo always saved to both the main save file and to the profile savefile
   that is why we do both at once here */
void gamestate_write_to_files_hook() {
    char path[PATH_CHARS];

    // Find the path where the main savefile goes.

    strncpy(path, profile_path(), PATH_CHARS);
    strncat(path, SAVE_PATH,      PATH_CHARS);

    // Dump to the main savefile extension

    game_state_upgrade_write_to_file(path);

    // Find the path where profile specific savefiles go

    memset(path, 0, PATH_CHARS);
    saved_game_file_get_path_to_enclosing_directory(active_profile_id(), path);
    strncat(path, SAVE_PATH, PATH_CHARS);

    // Dump to the profile specific savefile extension

    game_state_upgrade_write_to_file(path);
}

extern "C" __attribute__((regparm(2)))
char gamestate_copy_checkpoint_file_hook(char* to, char* base_path, char* from) {
    char return_value = gamestate_copy_checkpoint_file(to, base_path, from);
    // If the function returns 1 that means it succesfully copied the files
    if (!return_value) {
        return 0;
    }

    // We also need to copy ours.
    // This is basically sort of a copy of what Halo does, but for our files.

    char full_path1[PATH_CHARS];
    char full_path2[PATH_CHARS];

    snprintf(full_path1, PATH_CHARS, "%s%s.vulpes", base_path, from);
    snprintf(full_path2, PATH_CHARS, "%s%s.vulpes", base_path, to);

    CopyFile(full_path2, full_path1, 0);

    return 1;
}

static void gamestate_copy_to_backup_buffer_hook() {
    // Copy the vanilla gamestate into the checkpoint buffer.
    // We're replacing the vanilla mechanism for this because there isn't
    // really a great place to hook this function. So, we replaced the code
    // that normally handles this.
    memcpy(
        reinterpret_cast<void*>(*game_state_globals_autosave_thread),
        reinterpret_cast<void*>(*game_state_globals),
        *game_state_globals_buffer_size
    );
    // Copy our gamestate upgrade to our checkpoint buffer.
    memcpy(
        gamestate_extension_checkpoint_buffer,
        gamestate_extension_buffer,
        ALLOCATED_UPGRADE_MEMORY
    );
}

static Patch(patch_gamestate_new_replacement, NULL, 6,
    JMP_PATCH, &gamestate_table_new_wrapper);

static Patch(patch_gamestate_read_from_main_file_hook, NULL, 5,
    JMP_PATCH, &gamestate_read_from_main_file_hook_wrapper);

static Patch(patch_gamestate_read_from_profile_file_hook, NULL, 6,
    JMP_PATCH, &gamestate_read_from_profile_file_hook_wrapper);

static Patch(patch_gamestate_write_to_files_hook, NULL, 5,
    CALL_PATCH, &gamestate_write_to_files_hook_wrapper);

static Patch(patch_copy_to_checkpoint_state_hook, NULL, 9,
    CALL_PATCH, &gamestate_copy_to_backup_buffer_hook);

static Patch(patch_copy_checkpoint_file_hook, NULL, 6,
    JMP_PATCH, &gamestate_copy_checkpoint_file_wrapper);

static bool initialized = false;

void init_gamestate_upgrades() {
    if (initialized) return;

    game_state_globals = *sig_game_state_globals();
    game_state_globals_cpu_allocation_size = *sig_game_state_globals_cpu_allocation_size();
    game_state_globals_autosave_thread = *sig_game_state_globals_autosave_thread();
    game_state_globals_buffer_size = *sig_game_state_globals_buffer_size();
    game_state_globals_crc = *sig_game_state_globals_crc();

    // Patch the original table allocation function to replace it with ours.
    patch_gamestate_new_replacement.build(sig_game_state_data_new());
    patch_gamestate_new_replacement.apply();

    // Make sure to also load our file when the game loads from the main checkpoint file.
    patch_gamestate_read_from_main_file_hook.build(sig_game_state_read_from_main_file_hook());
    patch_gamestate_read_from_main_file_hook.apply();

    // Also load our file from the player specific checkpoint folder when the game does so.
    patch_gamestate_read_from_profile_file_hook.build(sig_game_state_read_from_profile_file_hook());
    patch_gamestate_read_from_profile_file_hook.apply();

    // Hook into a small piece of code in the checkpoint file writing code
    // so we can execute our file writing code.
    patch_gamestate_write_to_files_hook.build(sig_game_state_write_to_file_patch());
    patch_gamestate_write_to_files_hook.apply();

    // Replace parts of the code that handles the memory copy to the checkpoint
    // buffer with our own code.
    patch_copy_to_checkpoint_state_hook.build(sig_game_state_copy_to_checkpoint_buffer());
    patch_copy_to_checkpoint_state_hook.apply();

    // Extend the checkpoint file copying function to also copy our checkpoint
    // file.
    patch_copy_checkpoint_file_hook.build(sig_copy_checkpoint_file_hook());
    patch_copy_checkpoint_file_hook.apply();
    gamestate_copy_checkpoint_file_continue_ptr = patch_copy_checkpoint_file_hook.return_address();

    // This is the location the game stores its gamestate in.
    uintptr_t mem_map_loc = *sig_physical_memory_map_location();

    // Allocate and null the memory for our upgrades.
    gamestate_extension_buffer = VirtualAlloc(
        // Allocating on a static address relieves us from the duty of needing
        // to patch loaded stuff to fit in the current memory model.
        // We basically allocate our chunk of gamestate before vanilla gamestate.
        reinterpret_cast<void*>(mem_map_loc-ALLOCATED_UPGRADE_MEMORY),
        ALLOCATED_UPGRADE_MEMORY,
        MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    if (!gamestate_extension_buffer)
        printf("Failed to allocate upgraded gamestate.\n"
               "VirtualAlloc Error Code: %d\n", GetLastError());

    gamestate_extension_checkpoint_buffer = VirtualAlloc(
        NULL,
        ALLOCATED_UPGRADE_MEMORY,
        MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    if (!gamestate_extension_checkpoint_buffer)
        printf("Failed to allocate upgraded gamestate checkpoint buffer.\n"
               "VirtualAlloc Error Code: %d\n", GetLastError());

    assert(gamestate_extension_buffer);
    assert(gamestate_extension_checkpoint_buffer);

    memset(gamestate_extension_buffer, 0, ALLOCATED_UPGRADE_MEMORY);
}

void revert_gamestate_upgrades() {
    if (!initialized) return;

    patch_copy_checkpoint_file_hook.revert();
    patch_copy_to_checkpoint_state_hook.revert();
    patch_gamestate_write_to_files_hook.revert();
    patch_gamestate_read_from_profile_file_hook.revert();
    patch_gamestate_read_from_main_file_hook.revert();
    patch_gamestate_new_replacement.revert();

    VirtualFree(gamestate_extension_buffer, ALLOCATED_UPGRADE_MEMORY, MEM_RELEASE);
    VirtualFree(gamestate_extension_checkpoint_buffer, ALLOCATED_UPGRADE_MEMORY, MEM_RELEASE);

}
