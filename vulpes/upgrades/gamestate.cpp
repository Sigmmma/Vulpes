/*
 * Vulpes (c) 2020 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <cstdio>
#include <windows.h>

#include <hooker/hooker.hpp>
#include <vulpes/memory/signatures.hpp>
#include <vulpes/memory/gamestate/table.hpp>

#include "gamestate.hpp"

static void table_set_up(GenericTable* table, char* name, uint32_t element_size, uint32_t element_max) {
    memset(new_table, 0, sizeof(GenericTable));
    strncpy(new_table->name, name, sizeof(new_table->name));
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
        char* name, uint32_t element_size, uint32_t element_max) {
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
    table->first = reinterpret_cast<void*>(
            reinterpret_cast<uintptr_t>(new_table) + sizeof(GenericTable));

    return new_table;
}

static size_t used_tables = 0;
static size_t used_memory = 0;
static const size_t ALLOCATED_MEMORY = 2*1024*1024;
static void* gamestate_extension_buffer;

static GenericTable tables[32];

// Wonky argument order needed for the wrapper to stay simple.
extern "C" __attribute__((regparm(1)))
GenericTable* game_state_table_new_replacement(uint32_t element_size,
        char* name, uint32_t element_max) {
    auto new_table = gamestate_table_new_vanilla_memory(name, element_size, element_max);
    return new_table;
}

extern "C" void game_state_table_new_wrapper();

static Patch(patch_gamestate_new_replacement, NULL, 6,
    JMP_PATCH, &game_state_table_new_wrapper);

void init_gamestate_upgrades() {
    auto patch_addr = sig_game_state_data_new();
    game_state_globals = *reinterpret_cast<uint32_t**>(patch_addr+2);
    game_state_globals_cpu_allocation_size = *reinterpret_cast<uint32_t**>(patch_addr+0x37);
    patch_gamestate_new_replacement.build(patch_addr);
    gamestate_new_return_address = patch_gamestate_new_replacement.return_address();
    patch_gamestate_new_replacement.apply();
}
void revert_gamestate_upgrades() {
    patch_gamestate_new_replacement.revert();
}
