#pragma once
#include <cstdint>

__attribute__((cdecl))
void auto_complete(char* buffer[], uint16_t* current_index, const char* input);


__attribute__((cdecl))
bool process_command(char* input);
