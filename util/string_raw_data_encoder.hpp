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
#include <cstring>
#include <cstdint>
extern "C" {
    // Encodes an array of 16 bit ints with the size len into a wchar_t array.
    // Adding encoding info at the end.
    // Warning: Output len can be up to len*16/15 as long as input (longer!).
    size_t wstr_raw_data_encode(wchar_t* dest, const uint16_t* src, size_t len);

    // Decodes a wchar_t array into a uint16_t array.
    void   wstr_raw_data_decode(uint16_t* dest, const wchar_t* src, size_t len);
}
