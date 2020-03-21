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

static void table_set_up(GenericTable* table, const char* name, uint32_t element_size, uint32_t element_max) {
    memset(table, 0, sizeof(GenericTable));
    strncpy(table->name, name, sizeof(table->name) - 1);
    table->max_elements = element_max;
    table->element_size = element_size;
    // Just hex for the NOT terminated string 'd@t@'
    table->sig = 0x64407440; //'d@t@'
    // Mark as invalid as that is what vanilla does.
    table->is_valid = false;
}

// Vanilla

static uint32_t* game_state_globals;
static uint32_t* game_state_globals_cpu_allocation_size;
uint32_t gamestate_new_return_address;

// We're replacing the stock table allocation function for no reason other
// than that I have been in stack debug hell for three hours now.
// And I got shit to do.
// So, this here does almost exactly what stock does.
// Except for things I stripped for speed (tm)
static GenericTable* gamestate_table_new_vanilla_memory(
        const char* name, uint32_t element_size, uint32_t element_max) {
    // Get the next available spot in vanilla gamestate.
    auto new_table = reinterpret_cast<GenericTable*>(
            *game_state_globals + *game_state_globals_cpu_allocation_size);

    // Do generic table setup.
    table_set_up(new_table, name, element_size, element_max);

    // Calculate total used vanilla gamestate.
    auto new_alloc_size = *game_state_globals_cpu_allocation_size
                          + sizeof(GenericTable)
                          + element_size * element_max;
    // Store the total.
    *game_state_globals_cpu_allocation_size = new_alloc_size;

    // The first entry in the table in vanilla gamestate is right after the
    // header.
    new_table->first_int = reinterpret_cast<uintptr_t>(new_table) + sizeof(GenericTable);

    return new_table;
}

static size_t used_tables = 0;
static size_t used_memory = 0;
static const size_t ALLOCATED_UPGRADE_MEMORY = 5*1024*1024;
static void* gamestate_extension_buffer;
static void* gamestate_extension_checkpoint_buffer;

static const size_t UPGRADE_TABLES = 32;
static GenericTable tables[UPGRADE_TABLES];
static GenericTable tables_checkpoint[UPGRADE_TABLES];

static GenericTable* gamestate_table_new_upgrade_memory(
        const char* name, uint32_t element_size, uint32_t element_max) {
    // Pick the next available table.
    size_t i = used_tables;
    used_tables++;

    // Never should we have more tables than the amount we allocated. Duh.
    assert(i < UPGRADE_TABLES);

    auto new_table = &tables[i];
    // Do generic table setup.
    table_set_up(new_table, name, element_size, element_max);

    // Make the first element be the next available spot in memory.
    new_table->first_int = reinterpret_cast<size_t>(gamestate_extension_buffer) + used_memory;

    // Update the used memory.
    used_memory += element_size * element_max;

    printf("Used upgrade memory: %d/%d\n", used_memory, ALLOCATED_UPGRADE_MEMORY);

    return new_table;
}

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

    {"flag", 16, true}, // original 2
    {"antenna", 24, true}, // original 12
    {"glow", 16, true}, // original 8
    {"glow particles", 1024, true}, // original 512
    {"light volumes", 1024, true}, // original 256
    {"lights", 2048, true}, // original 896
    {"players", 32, true}, // original 16
    {"teams", 32, true}, // original 16
    {"contrail", 1024, true}, // original 256
    {"contrail point", 4096, true}, // original 1024
    //{"particle", 2048, true}, // We have to wait until we can fix the game's particle code for this.
    {"effect", 4096, true}, // original 256
    {"effect location", 8192, true}, // original 512
    {"particle systems", 1024, true}, // original 64
    {"particle system particles", 4096, true}, // original 512
    {"actors", 1024, true}, // original 256
    {"swarm", 128, true}, // original 32
    {"swarm component", 1024, true}, // original 256
    {"prop", 768*4, true}, // original 768
    {"encounter", 1024, true}, // original 128
    {"ai persuit", 1024, true}, // original 256
    {"", 0, false}, // Terminate
};

struct CheckpointFileHeader {
    uint32_t memory_layout_crc;
    uint32_t tables_offset;
    uint32_t tables_count;
    uint32_t arrays_offset;
    uint32_t arrays_size;
    uint32_t reserved[5];
};

// Wonky argument order needed for the assembly wrapper to stay simple.
extern "C" __attribute__((regparm(1)))
GenericTable* gamestate_table_new_replacement(uint32_t element_size,
        const char* name, uint16_t element_max) {
    GenericTable* new_table;
    printf(
        "Table creation request:\n"
        "name:%-48s element_size:%8d element_max:%8d\n", name, element_size, element_max);

    bool found = false;
    bool use_upgrade_memory = false;
    size_t i = 0;
    while (!found && TABLE_UPGRADES[i].new_max != 0) {
        if (strncmp(name, TABLE_UPGRADES[i].name, 31) == 0) {
            element_max = TABLE_UPGRADES[i].new_max;
            use_upgrade_memory = TABLE_UPGRADES[i].in_upgrade_memory;
            printf("Upgrading element_max to %d\n", element_max);
            found = true;
            break;
        }
        i++;
    }

    if (use_upgrade_memory) {
        new_table = gamestate_table_new_upgrade_memory(name, element_size, element_max);
    } else {
        new_table = gamestate_table_new_vanilla_memory(name, element_size, element_max);
    }

    return new_table;
}

static void save_checkpoint_upgrade() {
    memcpy(gamestate_extension_checkpoint_buffer, gamestate_extension_buffer, ALLOCATED_UPGRADE_MEMORY);
    memcpy(tables_checkpoint, tables, sizeof(tables));

    uintptr_t upgrade_start_address_int = reinterpret_cast<uintptr_t>(gamestate_extension_buffer);
    for (int i=0; i < UPGRADE_TABLES; i++) {
        // Get the offset so that change in memory address does not break saves.
        uintptr_t array_start_address_int = tables_checkpoint[i].first_int;
        uintptr_t offset = array_start_address_int - upgrade_start_address_int;

        tables_checkpoint[i].first_int = offset;
    }

    auto save_file_path = std::string(profile_path()) + "\\savegame.vulpes";

    FILE* save_file = fopen(save_file_path.c_str(), "wb");

    // Prepare save_file header.
    CheckpointFileHeader header;
    memset(&header, 0, sizeof(header));
    header.memory_layout_crc = 0;
    header.tables_count = used_tables;
    header.tables_offset = sizeof(header);
    header.arrays_offset = header.tables_offset + sizeof(tables);
    header.arrays_size = used_memory;

    // Write all the relevant data to the file.
    fwrite(&header, 1, sizeof(header), save_file);
    fwrite(&tables_checkpoint, 1, sizeof(tables_checkpoint), save_file);
    fwrite(gamestate_extension_buffer, 1, used_memory, save_file);

    fflush(save_file);
    fclose(save_file);
}

static void load_checkpoint_upgrade() {
    uintptr_t upgrade_start_address_int = reinterpret_cast<uintptr_t>(gamestate_extension_buffer);
    for (int i=0; i < UPGRADE_TABLES; i++) {
        // Get the offset so that change in memory address does not break saves.
        uintptr_t array_start_offset_int = tables_checkpoint[i].first_int;
        uintptr_t address = upgrade_start_address_int + array_start_offset_int;

        tables_checkpoint[i].first_int = address;
    }

    memcpy(gamestate_extension_buffer, gamestate_extension_checkpoint_buffer, ALLOCATED_UPGRADE_MEMORY);
    memcpy(tables, tables_checkpoint, sizeof(tables));
}

extern "C" void gamestate_table_new_wrapper();

static Patch(patch_gamestate_new_replacement, NULL, 6,
    JMP_PATCH, &gamestate_table_new_wrapper);

static bool initialized = false;

void init_gamestate_upgrades() {
    if (initialized) return;

    auto patch_addr = sig_game_state_data_new();

    game_state_globals = *reinterpret_cast<uint32_t**>(patch_addr+2);
    game_state_globals_cpu_allocation_size = *reinterpret_cast<uint32_t**>(patch_addr+0x37);

    patch_gamestate_new_replacement.build(patch_addr);
    gamestate_new_return_address = patch_gamestate_new_replacement.return_address();
    patch_gamestate_new_replacement.apply();

    // Allocate and null the memory for our upgrades.
    gamestate_extension_buffer = VirtualAlloc(NULL, ALLOCATED_UPGRADE_MEMORY,
        MEM_COMMIT, PAGE_READWRITE);
    gamestate_extension_checkpoint_buffer = VirtualAlloc(NULL, ALLOCATED_UPGRADE_MEMORY,
        MEM_COMMIT, PAGE_READWRITE);

    ADD_CALLBACK_P(EVENT_BEFORE_SAVE, save_checkpoint_upgrade, EVENT_PRIORITY_FINAL);
    ADD_CALLBACK_P(EVENT_BEFORE_LOAD, load_checkpoint_upgrade, EVENT_PRIORITY_FINAL);
}

void revert_gamestate_upgrades() {
    if (!initialized) return;

    patch_gamestate_new_replacement.revert();

    VirtualFree(gamestate_extension_buffer, ALLOCATED_UPGRADE_MEMORY, MEM_RELEASE);
    VirtualFree(gamestate_extension_checkpoint_buffer, ALLOCATED_UPGRADE_MEMORY, MEM_RELEASE);

    DEL_CALLBACK(EVENT_BEFORE_SAVE, save_checkpoint_upgrade);
    DEL_CALLBACK(EVENT_BEFORE_LOAD, load_checkpoint_upgrade);
}
