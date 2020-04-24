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

#include <vulpes/memory/types.hpp>

#include "table.hpp"

#pragma pack(push, 1)

class ConsoleOutput : public BaseTableEntry {
public:
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
