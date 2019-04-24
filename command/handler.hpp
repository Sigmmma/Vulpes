#pragma once
#include <cstdint>

__attribute__((cdecl))
int16_t auto_complete(char** const buffer, int16_t current_index, const char* const input);
