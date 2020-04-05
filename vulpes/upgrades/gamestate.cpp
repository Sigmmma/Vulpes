/*
 * Vulpes (c) 2020 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <cassert>
#include <cstdio>
#include <cstring>
#include <windows.h> /* VirtualAlloc, VirtualFree */

#include <hooker/hooker.hpp>
#include <vulpes/memory/global.hpp>
#include <vulpes/memory/signatures.hpp>
#include <vulpes/memory/gamestate/table.hpp>

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
    {"", 0, false}, // Terminate
};

// Wonky argument order needed for the assembly wrapper to stay simple.
extern "C" __attribute__((regparm(1)))
GenericTable* gamestate_table_new_replacement(uint32_t element_size,
        const char* name, uint16_t element_max) {

    bool found = false;
    bool use_upgrade_memory = false;
    size_t i = 0;
    while (!found && TABLE_UPGRADES[i].new_max != 0) {
        if (strncmp(name, TABLE_UPGRADES[i].name, 31) == 0) {
            // Set max to new max.
            if (TABLE_UPGRADES[i].new_max > element_max) {
                // Only use our limit if it is higher. Other mods may increase
                // limits more than us and we have extra memory allocated for
                // the occasion.
                element_max = TABLE_UPGRADES[i].new_max;
            }
            use_upgrade_memory = TABLE_UPGRADES[i].in_upgrade_memory;
            found = true;
            break;
        }
        i++;
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
    assert(*mem_used <= *mem_max);

    new_table->init(
        name, element_max, element_size,
        reinterpret_cast<uintptr_t>(new_table) + sizeof(GenericTable)
    );

    return new_table;
}

static void game_state_upgrade_read_from_file(char* filepath) {
    FILE* save_file = fopen(filepath, "rb");
    // Read the upgrade memory from file.
    fread(gamestate_extension_buffer, 1, ALLOCATED_UPGRADE_MEMORY, save_file);

    // Close the file.
    fclose(save_file);
}

static void game_state_upgrade_write_to_file(char* filepath) {
    FILE* save_file = fopen(filepath, "wb");
    // Just dump the entire upgrade memory into one file.
    fwrite(gamestate_extension_checkpoint_buffer, 1, ALLOCATED_UPGRADE_MEMORY, save_file);
    // Flush and close.
    fflush(save_file);
    fclose(save_file);
}

extern "C"
void gamestate_read_from_main_file_hook() {
    char path[PATH_CHARS];

    // Find the path where the main savefile goes.

    strcpy(path, profile_path());
    strcat(path, SAVE_PATH);

    // Read directly from the main savefile extension

    game_state_upgrade_read_from_file(path);
}

extern "C"
void gamestate_read_from_profile_file_hook() {
    char path[PATH_CHARS];

    // Find the path where the main savefile goes.

    saved_game_file_get_path_to_enclosing_directory(active_profile_id(), path);
    strcat(path, SAVE_PATH);

    // Read directly from the main savefile extension

    game_state_upgrade_read_from_file(path);
}

extern "C"
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
char gamestate_copy_checkpoint_file_hook(char* a1, char* a2, char* a3) {
    char return_value = gamestate_copy_checkpoint_file(a1, a2, a3);
    // If the function returns 1 that means it succesfully copied the files
    if (!return_value) {
        return 0;
    }

    // We also need to copy ours.
    // This is basically sort of a copy of what Halo does, but for our files.

    char full_path1[PATH_CHARS];
    char full_path2[PATH_CHARS];

    snprintf(full_path1, PATH_CHARS, "%s%s.vulpes", a2, a3);
    snprintf(full_path2, PATH_CHARS, "%s%s.vulpes", a2, a1);

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

    uintptr_t game_state_table_alloc_patch_addr = sig_game_state_data_new();
    uintptr_t write_to_file_patch_addr = sig_game_state_write_to_file();
    uintptr_t copy_to_checkpoint_state_patch_addr = sig_game_state_copy_to_checkpoint_buffer();

    game_state_globals = *reinterpret_cast<uintptr_t**>(game_state_table_alloc_patch_addr+2);
    game_state_globals_cpu_allocation_size = *reinterpret_cast<uintptr_t**>(game_state_table_alloc_patch_addr+0x37);
    game_state_globals_autosave_thread = *reinterpret_cast<void***>(write_to_file_patch_addr+21);
    game_state_globals_buffer_size = *reinterpret_cast<const uintptr_t**>(write_to_file_patch_addr+15);
    //game_state_globals_file_handle = *reinterpret_cast<HANDLE**>(write_to_file_patch_addr+2);

    // Patch the original table allocation function to replace it with ours.
    patch_gamestate_new_replacement.build(game_state_table_alloc_patch_addr);
    patch_gamestate_new_replacement.apply();

    // Make sure to also load our file when the game loads from the main checkpoint file.
    patch_gamestate_read_from_main_file_hook.build(sig_game_state_read_from_main_file_hook());
    patch_gamestate_read_from_main_file_hook.apply();

    // Also load our file from the player specific checkpoint folder when the game does so.
    patch_gamestate_read_from_profile_file_hook.build(sig_game_state_read_from_profile_file_hook());
    patch_gamestate_read_from_profile_file_hook.apply();

    // Hook into a small piece of code in the checkpoint file writing code
    // so we can execute our file writing code.
    patch_gamestate_write_to_files_hook.build(write_to_file_patch_addr + 51);
    patch_gamestate_write_to_files_hook.apply();

    // Replace parts of the code that handles the memory copy to the checkpoint
    // buffer with our own code.
    patch_copy_to_checkpoint_state_hook.build(copy_to_checkpoint_state_patch_addr);
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
    gamestate_extension_checkpoint_buffer = VirtualAlloc(
        NULL,
        ALLOCATED_UPGRADE_MEMORY,
        MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    assert(gamestate_extension_buffer);
    assert(gamestate_extension_checkpoint_buffer);

    memset(gamestate_extension_buffer, 0, ALLOCATED_UPGRADE_MEMORY);
}

void revert_gamestate_upgrades() {
    if (!initialized) return;

    patch_gamestate_new_replacement.revert();
    patch_gamestate_write_to_files_hook.revert();
    patch_copy_to_checkpoint_state_hook.revert();
    patch_gamestate_read_from_main_file_hook.revert();
    patch_gamestate_read_from_profile_file_hook.revert();
    patch_copy_to_checkpoint_state_hook.revert();

    VirtualFree(gamestate_extension_buffer, ALLOCATED_UPGRADE_MEMORY, MEM_RELEASE);
    VirtualFree(gamestate_extension_checkpoint_buffer, ALLOCATED_UPGRADE_MEMORY, MEM_RELEASE);

}
