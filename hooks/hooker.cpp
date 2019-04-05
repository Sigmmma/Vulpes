#include "hooker.hpp"

#define WIN32_MEAN_AND_LEAN // Keeps the amount of windows.h includes to the bare minumum.
#include <Windows.h>
#include <cassert>
#include <string>
#include <cstdio>

uintptr_t CodeSignature::get_address(){
    if (address == 0 && !already_tried){
        if (!lowest_allowed){
            lowest_allowed = get_lowest_permitted_address();
        };
        if (!highest_allowed){
            highest_allowed = get_highest_permitted_address();
        };
        printf("Searching for CodeSignature %s between %X and %X...", name, lowest_allowed, highest_allowed);
        size_t size_of_block_to_find = sig.size();
        uintptr_t current_address = lowest_allowed;
        // Traverse from the lowest to highest address allowed searching for the set of bytes that we want.
        while (address == 0 && current_address - size_of_block_to_find <= highest_allowed){
            bool mismatch = false;
            // For each address we go through our set of bytes until we get a mismatch or we reach the end of our signature.
            for (int j=0; j < size_of_block_to_find; j++){
                // If the current element in our sig is -1 we skip this byte as -1 is our wildcard.
                // If our current byte matches our current sig element, start the next iteration.
                if (sig[j] == -1 || reinterpret_cast<uint8_t*>(current_address)[j] == sig[j]){
                    continue;
                };
                // If neither of the conditions are met we have a mismatch and should move on.
                mismatch = true;
                break;
            };
            // If there was no mismatch then we have succesfully found the address and we can go home.
            if (!mismatch){
                address = current_address;
            };
            current_address++;
        };
    };
    if (address == 0){
        printf("failed\n");
        if (imperative){
            // Crash
        };
    }else{
        printf("success. Found at %X\n", address);
    }
    return address;
}

uintptr_t CodeSignature::get_address(bool recalculate){
    if (recalculate){
        address = 0;
        already_tried = false;
    };
    return get_address();
}

////////////

void CodePatch::build(uintptr_t p_address, size_t p_size, PatchTypes p_type, uintptr_t redirect_to){
    printf("Building CodePatch %s...", name);
    assert(redirect_to >= get_lowest_permitted_address());
    patch_address = p_address; size = p_size; type = p_type; redirect_address = redirect_to;

    applied = false;
    return_address = patch_address + size;

    uint8_t* patch_address_bytes = reinterpret_cast<uint8_t*>(patch_address);
    // Make a copy of the old code
    for (int i = 0; i < size; i++){
        original_code.push_back(patch_address_bytes[i]);
    };

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
            assert(size >= 5);
            patched_code.push_back(CALL_BYTE);
            used_area = 5;
            write_pointer = true;
            break;
        case JMP_PATCH :
            assert(size >= 5);
            patched_code.push_back(JMP_BYTE);
            used_area = 5;
            write_pointer = true;
            break;
        case SKIP_PATCH :
            if (size < 5){
                used_area = 0;
            }else if (size < 128){
                patched_code.push_back(JMP_SMALL_BYTE);
                patched_code.push_back(uint8_t(size - 2));
                used_area = 2;
            };
            write_pointer = false;
            break;
    };

    // Write pointer to our own code.
    if (write_pointer){
        // The version of the call and jmp instructions we use jump to the address
        // relative to the start of the instruction + the size of the instruction.
        uintptr_t relative_address_int = redirect_address - patch_address - 5;
        // Get the bytes that make up the address in the order that they are stored in memory.
        uint8_t* relative_address_bytes = reinterpret_cast<uint8_t*>(&relative_address_int);
        for (int i = 0; i < 4; i++){
            patched_code.push_back(relative_address_bytes[i]);
        };
    };

    // Pad remaining bytes.
    for (int i = used_area; i < size; i++){
        patched_code.push_back(NOP_BYTE);
    };
    patch_is_built = true;
    printf("done\n");
}

void CodePatch::write_patch(std::vector<uint8_t> patch_code){
    assert(patch_is_built);
    uint8_t* patch_address_bytes = reinterpret_cast<uint8_t*>(patch_address);
    DWORD prota, protb;
    VirtualProtect(reinterpret_cast<void*>(patch_address), size, PAGE_READWRITE, &prota);
    for (int i = 0; i < size; i++){
        patch_address_bytes[i] = patch_code[i];
    };
    VirtualProtect(reinterpret_cast<void*>(patch_address), size, prota, &protb);
}

void CodePatch::apply(){
    printf("Applying CodePatch %s...", name);
    assert(patch_is_built);
    write_patch(patched_code);
    applied = true;
    printf("done\n");
}

void CodePatch::revert(){
    printf("Reverting CodePatch %s...", name);
    assert(patch_is_built);
    write_patch(original_code);
    applied = false;
    printf("done\n");
}

bool CodePatch::check_integrity(){
    assert(patch_is_built);
    uint8_t* patch_address_bytes = reinterpret_cast<uint8_t*>(patch_address);
    std::vector<uint8_t> comparison_code;

    if (applied) {comparison_code = patched_code;}
    else         {comparison_code = original_code;};

    bool intact = true;
    for (int i = 0; i < size; i++){
        if (patch_address_bytes[i] == comparison_code[i]){
            continue;
        }else{
            intact = false;
            break;
        }
    };
    return intact;
}

bool CodePatch::is_applied(){
    assert(patch_is_built);
    return applied;
}

size_t CodePatch::get_size(){
    assert(patch_is_built);
    return size;
}

std::vector<uint8_t> CodePatch::get_bytes_from_patch_address(){
    assert(patch_is_built);
    uint8_t* patch_address_bytes = reinterpret_cast<uint8_t*>(patch_address);
    std::vector<uint8_t> found_code;
    for (int i = 0; i < size; i++){
        found_code.push_back(patch_address_bytes[i]);
    };
    return found_code;
}

std::vector<uint8_t> CodePatch::get_unpatched_bytes(){
    assert(patch_is_built);
    return original_code;
}

std::vector<uint8_t> CodePatch::get_patched_bytes(){
    assert(patch_is_built);
    return patched_code;
}

uintptr_t CodePatch::get_return_address(){
    assert(patch_is_built);
    return return_address;
}

uintptr_t get_call_address(uintptr_t call_pointer){
    uint8_t* call_bytes = reinterpret_cast<uint8_t*>(call_pointer);
    if (call_bytes[0] == 0xE8 || call_bytes[0] == 0xE9){
        return (*reinterpret_cast<uintptr_t*>(call_pointer + 1) + call_pointer + 5);
    };
    return 0;
}

static uintptr_t lowest_permitted_address = 0x400000;
static uintptr_t highest_permitted_address = 0x5DF000;
uintptr_t get_lowest_permitted_address(){
    return lowest_permitted_address;
}
uintptr_t get_highest_permitted_address(){
    return highest_permitted_address;
}
void set_lowest_permitted_address(uintptr_t new_address){
    lowest_permitted_address = new_address;
}
void set_highest_permitted_address(uintptr_t new_address){
    highest_permitted_address = new_address;
}
