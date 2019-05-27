/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#pragma once
#include "../types.hpp"
#include "table.hpp"

#pragma pack(push, 1)

class ConsoleOutput {
public:
    int16_t salt;
    PAD(2);
    MemRef previous;
    MemRef next;
    bool tab_stops;
    char text[255];
    PAD(4);
    ARGBFloat color;
    int32_t fade_frames;
}; static_assert(sizeof(ConsoleOutput) == 0x124);

class ConsoleOutputTable : public Table {
public:
    ConsoleOutput* entries;
};

struct ConsoleState {
    int32_t keys[33]; // lots of key info, int32_t is not the type.
    ARGBFloat color;
    char prefix[32];
    char token_buffer[256];
    struct {
        char* text;
        PAD(2);//unknown
        int16_t length;
        PAD(2);//unknown
        PAD(2);
    }edit_text;
};static_assert(sizeof(ConsoleState) == 0x1C0);

struct ConsoleInputGlobals {
    bool active;
    bool enabled; // Set by -console
    PAD(2);

    ConsoleState state;

    struct {
        char entries[8][255];
        int16_t inputs_saved;
        int16_t current_id;
        PAD(2); // unknown
        PAD(2);
    }history;
}; static_assert(sizeof(ConsoleInputGlobals) == 0x9C4);

struct ConsoleGlobals {
    bool initialized;
    PAD(3);
    ConsoleOutputTable* output;
    MemRef first;
    MemRef next;

    ConsoleState* current_state;

    bool receiving_input;
    PAD(3);
    uint32_t receiving_input_start_tick;
    bool active;
    PAD(3);
    int32_t rcon_machine_id;
};
#pragma pack(pop)

ConsoleGlobals* console_globals();
ConsoleOutputTable* console_output_table();
ConsoleInputGlobals* console_input_globals();
