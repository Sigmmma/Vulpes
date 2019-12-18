/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <cassert>
#include <cstdio>
#include <iostream>
#include <string>
#include <windows.h>

#include "hooker.hpp"

CodeSignature::CodeSignature(bool required,
                             const char* d_name,
                             uintptr_t lowest_search_address,
                             uintptr_t highest_search_address,
                             std::vector<int16_t> signature) {
    imperative = required;
    lowest_allowed = lowest_search_address;
    highest_allowed = highest_search_address;
    sig = signature;
    name = d_name;
}

uintptr_t CodeSignature::address(uintptr_t start_address, uintptr_t end_address) {
    if (found_address == 0 && !already_tried) {
        if (start_address) lowest_allowed = start_address;
        if (end_address) highest_allowed = end_address;
        if (!lowest_allowed) lowest_allowed = get_lowest_permitted_address();
        if (!highest_allowed) highest_allowed = get_highest_permitted_address();
        assert(lowest_allowed >= get_lowest_permitted_address());
        printf("Searching for CodeSignature %s...", name);
        size_t size_of_block_to_find = sig.size();
        uintptr_t current_address = lowest_allowed;
        // Traverse from the lowest to highest address allowed searching for the set of bytes that we want.
        while (found_address == 0 && current_address - size_of_block_to_find <= highest_allowed) {
            bool mismatch = false;
            // For each address we go through our set of bytes until we get a mismatch or we reach the end of our signature.
            for (int j=0; j < size_of_block_to_find; j++) {
                // If the current element in our sig is -1 we skip this byte as -1 is our wildcard.
                // If our current byte matches our current sig element, start the next iteration.
                if (sig[j] == -1 || reinterpret_cast<uint8_t*>(current_address)[j] == sig[j]) {
                    continue;
                }
                // If neither of the conditions are met we have a mismatch and should move on.
                mismatch = true;
                break;
            }
            // If there was no mismatch then we have succesfully found the address and we can go home.
            if (!mismatch) found_address = current_address;
            current_address++;
        }
    }
    if (found_address == 0) {
        printf("failed\n");
        if (imperative) {
            // Crash
        }
    } else {
        printf("success. Found at %X\n", found_address);
    }
    already_tried = true;
    return found_address;
}

uintptr_t CodeSignature::address(bool recalculate) {
    if (recalculate) {
        found_address = 0;
        already_tried = false;
    }
    return address();
}

uintptr_t LiteSignature::search(
        uintptr_t start_address, uintptr_t end_address) {

    if (!start_address) start_address = get_lowest_permitted_address();
    if (!end_address) end_address = get_highest_permitted_address();

    printf("Search for sig %s\nAddress range: %8X - %8X\n", name, start_address, end_address);

    // TODO: Use constant.
    uintptr_t result = UINTPTR_MAX;

    uintptr_t current_address = start_address;
    while (result == UINTPTR_MAX && current_address - size <= end_address) {
        bool mismatch = false;
        // For each address we go through our set of bytes until
        // we get a mismatch or we reach the end of our signature.
        for (int j=0; j < size; j++) {
            // If the current element in our sig is -1 we skip this byte as -1
            // is our wildcard. If our current byte matches our current sig
            // element, start the next iteration.
            if (bytes[j] == -1 || reinterpret_cast<uint8_t*>(current_address)[j] == bytes[j]) {
                continue;
            }
            // If neither of the conditions are met we have a mismatch and should move on.
            mismatch = true;
            break;
        }
        // If there was no mismatch then we have succesfully found the address.
        if (!mismatch) {
            result = current_address;
        }
        // If not, then keep searching.
        current_address++;
    }

    if (result == UINTPTR_MAX) {
        printf("Not found.\n\n");
    } else {
        printf("Found at: %8X\n\n", result);
    }

    return result;
}

std::vector<uintptr_t> LiteSignature::search_multiple(
        uintptr_t start_address, uintptr_t end_address) {

    if (!start_address) start_address = get_lowest_permitted_address();
    if (!end_address) end_address = get_highest_permitted_address();

    printf("Multi sig search for %s\n\n", name);

    std::vector<uintptr_t> addresses;
    // TODO: Use constant.
    uintptr_t last_result = 0;
    while(last_result != UINTPTR_MAX && start_address + size < end_address) {
        last_result = search(start_address, end_address);
        if (last_result != UINTPTR_MAX) {
            addresses.push_back(last_result);
        }
        start_address = last_result + 1;
    }
    printf("Found %d addresses.\n", addresses.size());

    return addresses;
}


////////////

void CodePatch::setup_internal(void* content, size_t c_size) {
    switch(type) {
        case JA_PATCH :
            assert(patch_size >= 6);
        case JMP_PATCH :
        case CALL_PATCH :
            assert(patch_size >= 5);
            redirect_address = *reinterpret_cast<intptr_t*>(content);
            break;
        case SKIP_PATCH :
            assert(patch_size >= 2);
            break;
        case INT_PATCH :
            assert(c_size == patch_size && c_size <= 4);
            for (int i=0; i<c_size;i++) {
                reinterpret_cast<uint8_t*>(&redirect_address)[i] =
                    reinterpret_cast<uint8_t*>(content)[i];
            }
            break;
        case MANUAL_PATCH :
            assert(c_size == patch_size);
            for (int i=0; i<c_size;i++) {
                patched_code.push_back(reinterpret_cast<uint8_t*>(content)[i]);
            }
            break;
        case NOP_PATCH :
            break;
    }
}

// Go to resource.hpp for the functional parts of the template based initializers.

CodePatch::CodePatch(const char* d_name,
          CodeSignature& p_sig, int p_sig_offset,
          std::vector<int16_t> patch_bytes) {
    name = d_name;
    sig = p_sig;
    offset = p_sig_offset;
    patch_size = patch_bytes.size();
    type = MANUAL_PATCH;
    patched_code = patch_bytes;
}
CodePatch::CodePatch(const char* d_name,
          intptr_t p_address,
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
    if (!patch_address && !patch_built) patch_address = sig.address() + offset;
    if (patch_address - offset <= 0) {
        printf("Couldn't build. Invalid address.");
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
            patched_code.push_back(CONDJ_BYTE);
            patched_code.push_back(JA_BYTE);
            used_area = 6;
            write_pointer = true;
            break;
        case SKIP_PATCH :
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
        case MANUAL_PATCH :
            assert(patch_size == patched_code.size());
            used_area = patch_size;
            write_pointer = false;
            break;
        case INT_PATCH :
            used_area = patch_size;
            for (int i=0; i<patch_size; i++) {
                patched_code.push_back(reinterpret_cast<uint8_t*>(&redirect_address)[i]);
            }
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

intptr_t CodePatch::address() {
    if (!patch_address) patch_address = sig.address();
    return patch_address;
}

void CodePatch::write_patch(std::vector<int16_t> patch_code) {
    assert(patch_built);
    uint8_t* patch_address_bytes = reinterpret_cast<uint8_t*>(patch_address);
    DWORD prota, protb;
    VirtualProtect(reinterpret_cast<void*>(patch_address), patch_size, PAGE_EXECUTE_READWRITE, &prota);
    for (int i = 0; i < patch_size; i++) {
        if (patch_code[i] != -1) {
            patch_address_bytes[i] = static_cast<uint8_t>(patch_code[i]);
        }
    }
    VirtualProtect(reinterpret_cast<void*>(patch_address), patch_size, prota, &protb);
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
