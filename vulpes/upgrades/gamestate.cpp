/*
 * Vulpes (c) 2020 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <cassert>
#include <cstdio>
#include <windows.h>
#include <string>

#include <hooker/hooker.hpp>
#include <util/file_helpers.hpp>
#include <vulpes/hooks/save_load.hpp>
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
static uintptr_t* game_state_globals_buffer_size;
// The filehandle to the savefile.
/* The reason we don't use this is that Halo resizes the checkpoint file to
   a specific size every time Halo starts.
   We could fix this, but I'd rather avoid the potential compatibility conflict
   we could have with other mods over this.
*/
//static HANDLE*    game_state_globals_file_handle;

// Upgrades

static uintptr_t used_extension_memory = 0;
static const size_t ALLOCATED_UPGRADE_MEMORY = 5*1024*1024;
static void* gamestate_extension_buffer;
static void* gamestate_extension_checkpoint_buffer;

struct TableUpgradeData {
    const char* name; // Name to apply this upgrade to.
    uint16_t new_max; // New max to use.
    bool in_upgrade_memory; // Whether or not to store this upgrade in upgrade
    // memory. This is important because some tables might contain pointers.
    // Our memory does not have a set location.
};

const uint16_t UPGRADED_OBJECT_LIMIT = 4096; // original 2048

const TableUpgradeData TABLE_UPGRADES[] = {
    // Object refs have pointers wheras other table users only refer to
    // things with MemRef
    {"object", UPGRADED_OBJECT_LIMIT, false},
    {"cluster collideable object reference", UPGRADED_OBJECT_LIMIT, false},
    {"collideable object cluster reference", UPGRADED_OBJECT_LIMIT, false},
    {"cluster noncollideable object reference", UPGRADED_OBJECT_LIMIT, false},
    {"noncollideable object cluster reference", UPGRADED_OBJECT_LIMIT, false},
    {"lights", 2048, false}, // original 896
    {"contrail", 512, false}, // original 256
    {"contrail point", 2048, false}, // original 1024
    {"encounter", 256, false}, // original 128
    {"light volumes", 1024, false}, // original 256


    {"decals", 2048, true}, // original 2048
    {"cached object render states", 256, true}, // original 256
    {"flag", 16, true}, // original 2
    {"antenna", 24, true}, // original 12
    {"glow", 16, true}, // original 8
    {"glow particles", 1024, true}, // original 512


    //{"players", 32, false}, // original 16
    //{"teams", 32, true}, // original 16

    {"particle", 1024, true}, // We have to wait until we can fix the game's particle code for this.
    {"effect", 4096, true}, // original 256
    {"effect location", 8192, true}, // original 512
    {"particle systems", 1024, true}, // original 64
    {"particle system particles", 4096, true}, // original 512
    {"actors", 1024, true}, // original 256
    //{"swarm", 128, true}, // original 32
    //{"swarm component", 1024, true}, // original 256
    // Puts some AI code in an infinite loop if loaded using our system.
    // Too big to upgrade in vanilla memory.
    //{"prop", 768*4, false}, // original 768
    // Cannot be reloaded without encounters breaking completely if outside vanilla memory.

    {"ai persuit", 1024, true}, // original 256
    {"", 0, false}, // Terminate
};

// Wonky argument order needed for the assembly wrapper to stay simple.
extern "C" __attribute__((regparm(1)))
GenericTable* gamestate_table_new_replacement(uint32_t element_size,
        const char* name, uint16_t element_max) {
    printf(
        "Table alloc:"
        "%-32s e_size:%-4d e_max:%-4d\n",
        name, element_size, element_max
    );

    bool found = false;
    bool use_upgrade_memory = false;
    size_t i = 0;
    while (!found && TABLE_UPGRADES[i].new_max != 0) {
        if (strncmp(name, TABLE_UPGRADES[i].name, 31) == 0) {
            // Set max to new max.
            element_max = TABLE_UPGRADES[i].new_max;
            use_upgrade_memory = TABLE_UPGRADES[i].in_upgrade_memory;
            printf("Set e_max to %d\n", element_max);
            found = true;
            break;
        }
        i++;
    }

    uintptr_t* mem_start;
    uintptr_t* mem_used;

    if (use_upgrade_memory) {
        mem_start = reinterpret_cast<uintptr_t*>(&gamestate_extension_buffer);
        mem_used  = &used_extension_memory;
    } else {
        mem_start = game_state_globals;
        mem_used  = game_state_globals_cpu_allocation_size;
    }

    // Get the next available spot in vanilla gamestate.
    auto new_table = reinterpret_cast<GenericTable*>(*mem_start + *mem_used);

    // Do the same setup that Halo does.
    memset(new_table, 0, sizeof(GenericTable));
    strncpy(new_table->name, name, sizeof(new_table->name) - 1);
    new_table->max_elements = element_max;
    new_table->element_size = element_size;
    // Just hex for the NOT terminated string 'd@t@'
    new_table->sig = 0x64407440; //'d@t@'
    // Mark as invalid as that is what vanilla does.
    new_table->is_valid = false;

    // The first entry in the table in vanilla gamestate is right after the
    // header.
    new_table->first_int = reinterpret_cast<uintptr_t>(new_table) + sizeof(GenericTable);
    // Null the whole array.
    memset(new_table->first, 0, element_size * element_max);

    // Calculate and store the total used.
    *mem_used = *mem_used + sizeof(GenericTable) + element_size * element_max;

    assert(*mem_used <= use_upgrade_memory ? ALLOCATED_UPGRADE_MEMORY : 0x440000);

    printf(
        "%s used budget %d/%d\n",
        use_upgrade_memory ? "Upgrade" : "Vanilla",
        *mem_used,
        use_upgrade_memory ? ALLOCATED_UPGRADE_MEMORY : 0x440000
    );

    return new_table;
}

static const char* SAVE_PATH = "\\savegame.vulpes";

extern "C" void load_checkpoint_upgrade() {
    auto SAVE_FILE_PATH = std::string(profile_path()) + SAVE_PATH;
    FILE* save_file = fopen(SAVE_FILE_PATH.c_str(), "rb");
    // Read the upgrade memory from file.
    fread(gamestate_extension_buffer, 1, ALLOCATED_UPGRADE_MEMORY, save_file);

    // Close the file.
    fclose(save_file);
}

extern "C" void gamestate_write_to_file_hook() {
    auto SAVE_FILE_PATH = std::string(profile_path()) + SAVE_PATH;
    FILE* save_file = fopen(SAVE_FILE_PATH.c_str(), "wb");
    // Just dump the entire upgrade memory into one file.
    fwrite(gamestate_extension_checkpoint_buffer, 1, ALLOCATED_UPGRADE_MEMORY, save_file);
    // Flush and close.
    fflush(save_file);
    fclose(save_file);
}

void gamestate_copy_to_backup_buffer_hook() {
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

extern "C" void gamestate_table_new_wrapper();
extern "C" void gamestate_write_to_file_hook_wrapper();

static Patch(patch_gamestate_new_replacement, NULL, 6,
    JMP_PATCH, &gamestate_table_new_wrapper);

static Patch(patch_gamestate_write_to_file_hook, NULL, 5,
    CALL_PATCH, &gamestate_write_to_file_hook_wrapper);

static Patch(patch_copy_to_checkpoint_state_hook, NULL, 9,
    CALL_PATCH, &gamestate_copy_to_backup_buffer_hook);

static bool initialized = false;

void init_gamestate_upgrades() {
    if (initialized) return;

    auto patch_addr = sig_game_state_data_new();
    uintptr_t write_to_file_patch_address = 0x53BD33;
    uintptr_t copy_to_checkpoint_state_patch_address = 0x53BA94;

    game_state_globals = *reinterpret_cast<uintptr_t**>(patch_addr+2);
    game_state_globals_cpu_allocation_size = *reinterpret_cast<uintptr_t**>(patch_addr+0x37);
    game_state_globals_autosave_thread = *reinterpret_cast<void***>(write_to_file_patch_address+21);
    game_state_globals_buffer_size = *reinterpret_cast<uintptr_t**>(write_to_file_patch_address+15);
    //game_state_globals_file_handle = *reinterpret_cast<HANDLE**>(write_to_file_patch_address+2);

    // Patch the original table allocation function to replace it with ours.
    patch_gamestate_new_replacement.build(patch_addr);
    patch_gamestate_new_replacement.apply();

    // Hook into a small piece of code in the checkpoint file writing code
    // so we can execute our file writing code.
    patch_gamestate_write_to_file_hook.build(write_to_file_patch_address + 51);
    patch_gamestate_write_to_file_hook.apply();

    // Replace parts of the code that handles the memory copy to the checkpoint
    // buffer with our own code.
    patch_copy_to_checkpoint_state_hook.build(copy_to_checkpoint_state_patch_address);
    patch_copy_to_checkpoint_state_hook.apply();

    // I couldn't find a good place to hook this in. But the BEFORE_LOAD event
    // works just fine. So, even though it is weird to do it in two different
    // ways. It's what we're doing for now.
    ADD_CALLBACK_P(EVENT_BEFORE_LOAD, load_checkpoint_upgrade, EVENT_PRIORITY_FINAL);

    // Allocate and null the memory for our upgrades.
    gamestate_extension_buffer = VirtualAlloc(
        // Allocating on a static address relieves us from the duty of needing
        // to patch loaded stuff to fit in the current memory model.
        reinterpret_cast<void*>(0x40000000-ALLOCATED_UPGRADE_MEMORY),
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
    patch_gamestate_write_to_file_hook.revert();
    patch_copy_to_checkpoint_state_hook.revert();

    VirtualFree(gamestate_extension_buffer, ALLOCATED_UPGRADE_MEMORY, MEM_RELEASE);
    VirtualFree(gamestate_extension_checkpoint_buffer, ALLOCATED_UPGRADE_MEMORY, MEM_RELEASE);

    DEL_CALLBACK(EVENT_BEFORE_LOAD, load_checkpoint_upgrade);
}
