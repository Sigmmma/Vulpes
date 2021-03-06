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

#include <cassert>
#include <cstdio>
#include <iostream>
#include <string>
#include <windows.h>

#include "hooker.hpp"

uintptr_t LiteSignature::search(
        uintptr_t start_address, uintptr_t end_address) {

    if (!start_address) start_address = get_lowest_permitted_address();
    if (!end_address) end_address = get_highest_permitted_address();

    printf("Search for sig %s\nAddress range: %8X - %8X\n",
        this->name, start_address, end_address);

    uintptr_t result = NULL;

    uintptr_t current_address = start_address;
    while (!result && current_address - this->size <= end_address) {
        bool mismatch = false;
        uint8_t* cur_bytes = reinterpret_cast<uint8_t*>(current_address);
        // For each address we go through our set of bytes until
        // we get a mismatch or we reach the end of our signature.
        for (size_t j=0; j < this->size && !mismatch; j++) {
            // If the current element in our sig is -1 we skip this byte as -1
            // is our wildcard.
            if (bytes[j] != -1 && cur_bytes[j] != this->bytes[j]) {
                mismatch = true;
            }
        }
        // If there was no mismatch then we have succesfully found the address.
        if (!mismatch) {
            result = current_address;
        }
        // If not, then keep searching.
        current_address++;
    }

    if (!result) {
        printf("Not found.\n\n");
    } else {
        printf("Found at: %8X\n\n", result);
    }

    return result;
}

std::vector<uintptr_t> LiteSignature::search_multiple(uintptr_t start_address,
                                                      uintptr_t end_address) {

    if (!start_address) start_address = get_lowest_permitted_address();
    if (!end_address) end_address = get_highest_permitted_address();

    printf("Multi sig search for %s\n\n", name);

    std::vector<uintptr_t> addresses;

    uintptr_t last_result = 1;
    while(last_result && start_address + size < end_address) {
        last_result = search(start_address, end_address);
        if (last_result) {
            addresses.push_back(last_result);
        }
        start_address = last_result + 1;
    }
    printf("Found %d addresses.\n", addresses.size());

    return addresses;
}


CodePatch::CodePatch(const char* d_name,
                     uintptr_t p_address,
                     std::vector<int16_t> patch_bytes) {
    name = d_name;
    patch_address = p_address;
    patch_size = patch_bytes.size();
    type = MANUAL_PATCH;
    patched_code = patch_bytes;
}


bool CodePatch::build(uintptr_t p_address) {
    if (patch_built) return true;
    printf("Build CodePatch %s\n", name);
    if (p_address && !patch_built) patch_address = p_address;
    if (!patch_address) {
        printf("Couldn't build. Invalid address.\n");
        return false;
    }
    assert(patch_address >= get_lowest_permitted_address());

    // Setup for the different types of patches.
    uint32_t used_area;
    bool write_pointer = false;
    switch (type) {
        default:
        case NOP_PATCH :
            used_area = 0;
            write_pointer = false;
            break;
        case CALL_PATCH :
            assert(patch_size >= 5);

            patched_code.push_back(CALL_BYTE);
            used_area = 5;
            write_pointer = true;
            break;
        case JMP_PATCH :
            patched_code.push_back(JMP_BYTE);
            used_area = 5;
            write_pointer = true;
            break;
        case JA_PATCH :
            assert(patch_size >= 6);

            patched_code.push_back(CONDJ_BYTE);
            patched_code.push_back(JA_BYTE);
            used_area = 6;
            write_pointer = true;
            break;
        case SKIP_PATCH :
            assert(patch_size >= 2);

            if(patch_size < 128) {
                patched_code.push_back(JMP_SMALL_BYTE);
                patched_code.push_back(uint8_t(patch_size - 2));
                used_area = 2;
                write_pointer = false;
            } else {
                patched_code.push_back(JMP_BYTE);
                used_area = 5;
                write_pointer = true;
                redirect_address = patch_address + patch_size;
            }
            break;
        case INT_PATCH :
            /* Int patches are manual patches */
        case MANUAL_PATCH :
            assert(patch_size == patched_code.size());
            used_area = patch_size;
            write_pointer = false;
            break;
    }

    // Write pointer to our own code.
    if (write_pointer) {
        // The version of the call and jmp instructions we use jump to the address
        // relative to the start of the instruction + the size of the instruction.
        uintptr_t relative_address_int = redirect_address - patch_address - used_area;
        // Get the bytes that make up the address in the order that they are stored in memory.
        uint8_t* relative_address_bytes = reinterpret_cast<uint8_t*>(&relative_address_int);
        for (int i = 0; i < 4; i++) {
            patched_code.push_back(relative_address_bytes[i]);
        }
    }

    // Pad remaining bytes.
    for (int i = used_area; i < patch_size; i++) {
        patched_code.push_back(NOP_BYTE);
    }

    // Copy the bytes found at the original address
    // Also mimic the mask that could have been passed in through
    // the manual patch constructor.
    uint8_t* patch_address_bytes = reinterpret_cast<uint8_t*>(patch_address);
    // Make a copy of the old code
    for (int i = 0; i < patch_size; i++) {
        if (patched_code[i] != -1) {
            original_code.push_back(patch_address_bytes[i]);
        } else {
            original_code.push_back(-1);
        }
    }

    patch_built = true;
    printf("Built.\n");
    return patch_built;
}

uintptr_t CodePatch::address() {
    return patch_address;
}

void CodePatch::write_patch(std::vector<int16_t> patch_code) {
    assert(patch_built);
    uint8_t* patch_address_bytes = reinterpret_cast<uint8_t*>(patch_address);
    DWORD prota, protb;
    VirtualProtect(
        reinterpret_cast<void*>(patch_address),
        patch_size,
        PAGE_EXECUTE_READWRITE,
        &prota
    );
    for (int i = 0; i < patch_size; i++) {
        if (patch_code[i] != -1) {
            patch_address_bytes[i] = static_cast<uint8_t>(patch_code[i]);
        }
    }
    VirtualProtect(
        reinterpret_cast<void*>(patch_address),
        patch_size,
        prota,
        &protb
    );
}

void CodePatch::apply() {
    printf("Apply CodePatch %s\n", name);
    assert(patch_built);
    write_patch(patched_code);
    patch_applied = true;
    printf("Applied.\n");
}

void CodePatch::revert() {
    printf("Revert CodePatch %s", name);
    if (patch_applied) {
        write_patch(original_code);
        printf("Reverted.\n");
    } else {
        printf("No need.\n");
    }
    patch_applied = false;
}

bool CodePatch::integrity() {
    assert(patch_built);
    uint8_t* patch_address_bytes = reinterpret_cast<uint8_t*>(patch_address);
    std::vector<int16_t> comparison_code;

    if (patch_applied) {comparison_code = patched_code;}
    else                 {comparison_code = original_code;}

    bool intact = true;
    for (int i = 0; i < patch_size; i++) {
        if (patch_address_bytes[i] == comparison_code[i]) {
            continue;
        } else {
            intact = false;
            break;
        }
    }
    return intact;
}

bool CodePatch::applied() {return patch_applied;}
size_t CodePatch::size() {return patch_size;}
bool CodePatch::is_built() {return patch_built;}

std::vector<int16_t> CodePatch::bytes_at_patch_address() {
    uint8_t* patch_address_bytes = reinterpret_cast<uint8_t*>(patch_address);
    std::vector<int16_t> found_code;
    for (int i = 0; i < patch_size; i++) {
        found_code.push_back(patch_address_bytes[i]);
    }
    return found_code;
}

std::vector<int16_t> CodePatch::unpatched_bytes() {
    assert(patch_built);
    return original_code;
}

std::vector<int16_t> CodePatch::patched_bytes() {
    assert(patch_built);
    return patched_code;
}

uintptr_t CodePatch::return_address() {
    assert(patch_built);
    return patch_address + patch_size;
}

uintptr_t get_call_address(intptr_t call_pointer) {
    uint8_t* call_bytes = reinterpret_cast<uint8_t*>(call_pointer);
    assert(call_bytes[0] == CALL_BYTE || call_bytes[0] == JMP_BYTE || call_bytes[0] == CONDJ_BYTE);
    if (call_bytes[0] == CALL_BYTE || call_bytes[0] == JMP_BYTE) {
        return (*reinterpret_cast<uintptr_t*>(call_pointer + 1) + call_pointer + 5);
    } else if(call_bytes[0] == CONDJ_BYTE) {
        return (*reinterpret_cast<uintptr_t*>(call_pointer + 2) + call_pointer + 6);
    }
    return 0;
}

void set_call_address(intptr_t call_pointer, intptr_t point_to) {
    DWORD prota, protb;
    uint8_t* call_bytes = reinterpret_cast<uint8_t*>(call_pointer);
    assert(call_bytes[0] == CALL_BYTE || call_bytes[0] == JMP_BYTE || call_bytes[0] == CONDJ_BYTE);
    if (call_bytes[0] == CALL_BYTE || call_bytes[0] == JMP_BYTE) {
        VirtualProtect(reinterpret_cast<void*>(call_pointer), 5, PAGE_EXECUTE_READWRITE, &prota);
        *reinterpret_cast<intptr_t*>(call_pointer + 1) = point_to - 5 - call_pointer;
        VirtualProtect(reinterpret_cast<void*>(call_pointer), 5, prota, &protb);
    } else if(call_bytes[0] == CONDJ_BYTE) {
        VirtualProtect(reinterpret_cast<void*>(call_pointer), 6, PAGE_EXECUTE_READWRITE, &prota);
        *reinterpret_cast<intptr_t*>(call_pointer + 2) = point_to - 6 - call_pointer;
        VirtualProtect(reinterpret_cast<void*>(call_pointer), 6, prota, &protb);
    }
}

static uintptr_t lowest_permitted_address = 0x400000;
static uintptr_t highest_permitted_address = 0x5DF000;
uintptr_t get_lowest_permitted_address() {
    return lowest_permitted_address;
}
uintptr_t get_highest_permitted_address() {
    return highest_permitted_address;
}
void set_lowest_permitted_address(uintptr_t new_address) {
    lowest_permitted_address = new_address;
}
void set_highest_permitted_address(uintptr_t new_address) {
    highest_permitted_address = new_address;
}
