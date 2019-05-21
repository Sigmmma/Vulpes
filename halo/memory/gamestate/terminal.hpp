#pragma once
#include "../types.hpp"
#include "table.hpp"

#pragma pack(push, 1)

class TerminalOutput {
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
}; static_assert(sizeof(TerminalOutput) == 0x124);

class TerminalOutputTable : public Table {
public:
    TerminalOutput* entries;
};

struct ConsoleGlobals {
    bool active;
    bool enabled; // Set by -console
    PAD(2);

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
    }state; static_assert(sizeof(ConsoleState) == 0x1C0);

    struct {
        char entries[8][255];
        int16_t inputs_saved;
        int16_t current_id;
        PAD(2); // unknown
        PAD(2);
    }history;
}; static_assert(sizeof(ConsoleGlobals) == 0x9C4);

#pragma pack(pop)

ConsoleGlobals* console_globals();
TerminalOutputTable* terminal_output_table();
