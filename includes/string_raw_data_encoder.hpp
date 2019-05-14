#pragma once

#include <cstring>
#include <cstdint>

// Encodes an array of 16 bit ints with the size len into a wchar_t array.
// Adding encoding info at the end.
// Warning: Output len can be up to len*16/15 as long as input (longer!).
size_t wstr_raw_data_encode(wchar_t* dest, const uint16_t* src, size_t len);

// Decodes a wchar_t array into a uint16_t array.
void   wstr_raw_data_decode(uint16_t* dest, const wchar_t* src, size_t len);
