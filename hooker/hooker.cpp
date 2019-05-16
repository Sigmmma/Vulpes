#include "hooker.hpp"
#define WIN32_MEAN_AND_LEAN // Keeps the amount of windows.h includes to the bare minumum.
#include <windows.h>
#include <cassert>
#include <string>
#include <cstdio>
#include <iostream>

CodeSignature::CodeSignature(bool required,
                             const char* d_name,
                             uintptr_t lowest_search_address,
                             uintptr_t highest_search_address,
                             std::vector<int16_t> signature){
    imperative = required;
    lowest_allowed = lowest_search_address;
    highest_allowed = highest_search_address;
    sig = signature;
    name = d_name;
}

uintptr_t CodeSignature::get_address(){
    if (address == 0 && !already_tried){
        if (!lowest_allowed){
            lowest_allowed = get_lowest_permitted_address();
        };
        if (!highest_allowed){
            highest_allowed = get_highest_permitted_address();
        };
        assert(lowest_allowed >= get_lowest_permitted_address());
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
    already_tried = true;
    return address;
}

uintptr_t CodeSignature::get_address(bool recalculate){
    if (recalculate){
        address = 0;
        already_tried = false;
    };
    return get_address();
}

uintptr_t CodeSignature::get_address(uintptr_t start_address){
    lowest_allowed = start_address;
    return get_address();
}

uintptr_t CodeSignature::get_address(uintptr_t start_address, uintptr_t end_address){
    lowest_allowed = start_address;
    highest_allowed = end_address;
    return get_address();
}

////////////

CodePatch::CodePatch(const char* d_name){
    name = d_name;
}

void CodePatch::setup_internal(size_t p_size, PatchTypes p_type, void* content, size_t c_size){
    size = p_size; type = p_type;
    switch(type){
        case JA_PATCH :
            assert(size >= 6);
        case JMP_PATCH :
        case CALL_PATCH :
            assert(size >= 5);
            redirect_address = *reinterpret_cast<intptr_t*>(content);
            break;
        case SKIP_PATCH :
            assert(size >= 2);
            break;
        case INT_PATCH :
            assert(c_size == size && c_size <= 4);
            for (int i=0; i<c_size;i++){
                reinterpret_cast<uint8_t*>(&redirect_address)[i] =
                    reinterpret_cast<uint8_t*>(content)[i];
            };
            break;
        case MANUAL_PATCH :
            assert(c_size == size);
            for (int i=0; i<c_size;i++){
                patched_code.push_back(reinterpret_cast<uint8_t*>(content)[i]);
            };
            break;
        case NOP_PATCH :
            break;
    };
}

bool CodePatch::build(intptr_t p_address){
    if (patch_is_built){
        return true;
    };
    printf("Building CodePatch %s...", name);
    if (p_address && !patch_is_built){
        patch_address = p_address;
    };
    if (!patch_address && !patch_is_built){
        patch_address = sig.get_address() + offset;
    };
    if (!patch_address){
        return false;
    };
    assert(patch_address >= get_lowest_permitted_address());

    return_address = patch_address + size;

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
            if(size < 128){
                patched_code.push_back(JMP_SMALL_BYTE);
                patched_code.push_back(uint8_t(size - 2));
                used_area = 2;
                write_pointer = false;
            }else{
                patched_code.push_back(JMP_BYTE);
                used_area = 5;
                write_pointer = true;
                redirect_address = patch_address + size;
            };
            break;
        case MANUAL_PATCH :
            assert(size == patched_code.size());
            break;
        case INT_PATCH :
            used_area = size;
            for (int i=0; i<size; i++){
                patched_code.push_back(reinterpret_cast<uint8_t*>(&redirect_address)[i]);
            };
            write_pointer = false;
            break;
    };

    // Write pointer to our own code.
    if (write_pointer){
        // The version of the call and jmp instructions we use jump to the address
        // relative to the start of the instruction + the size of the instruction.
        uintptr_t relative_address_int = redirect_address - patch_address - used_area;
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

    // Copy the bytes found at the original address
    // Also mimic the mask that could have been passed in through
    // the manual patch constructor.
    uint8_t* patch_address_bytes = reinterpret_cast<uint8_t*>(patch_address);
    // Make a copy of the old code
    for (int i = 0; i < size; i++){
        if (patched_code[i] != -1){
            original_code.push_back(patch_address_bytes[i]);
        }else{
            original_code.push_back(-1);
        };
    };

    patch_is_built = true;
    printf("done\n");
    return patch_is_built;
}

void CodePatch::build_old(uintptr_t p_address, size_t p_size, PatchTypes p_type, uintptr_t redirect_to){
    printf("Building CodePatch %s...", name);
    assert(p_address >= get_lowest_permitted_address());
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
        case JA_PATCH :
            assert(size >= 6);
            patched_code.push_back(CONDJ_BYTE);
            patched_code.push_back(JA_BYTE);
            used_area = 6;
            write_pointer = true;
            break;
        case SKIP_PATCH :
            if (size < 5){
                used_area = 0;
            }else if(size < 128){
                patched_code.push_back(JMP_SMALL_BYTE);
                patched_code.push_back(uint8_t(size - 2));
                used_area = 2;
            };
            write_pointer = false;
            break;
        case MANUAL_PATCH :
            used_area = patched_code.size();
            write_pointer = false;
            break;
    };

    // Write pointer to our own code.
    if (write_pointer){
        // The version of the call and jmp instructions we use jump to the address
        // relative to the start of the instruction + the size of the instruction.
        uintptr_t relative_address_int = redirect_address - patch_address - used_area;
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

void CodePatch::build_manual(uintptr_t p_address, std::vector<int16_t> patch_bytes){
    printf("Building CodePatch %s...", name);
    patch_address = p_address;
    type = MANUAL_PATCH;
    size = patch_bytes.size();

    applied = false;
    return_address = patch_address + size;

    uint8_t* patch_address_bytes = reinterpret_cast<uint8_t*>(patch_address);
    // Make a copy of the old code
    for (int i = 0; i < size; i++){
        if (patch_bytes[i] != -1){
            original_code.push_back(patch_address_bytes[i]);
        }else{
            original_code.push_back(-1);
        };
    };

    patched_code = patch_bytes;
    patch_is_built = true;
    printf("done\n");
}

void CodePatch::build_int(uintptr_t p_address, uint32_t patch_int){
    std::vector<int16_t> patch_bytes;
    uint8_t* int_patch_bytes = reinterpret_cast<uint8_t*>(&patch_int);
    for (int i = 0; i < 4; i++){
        patch_bytes.push_back(int_patch_bytes[i]);
    };
    build_manual(p_address, patch_bytes);
}

void CodePatch::build_int16(uintptr_t p_address, uint16_t patch_int){
    std::vector<int16_t> patch_bytes;
    uint8_t* int_patch_bytes = reinterpret_cast<uint8_t*>(&patch_int);
    for (int i = 0; i < 2; i++){
        patch_bytes.push_back(int_patch_bytes[i]);
    };
    build_manual(p_address, patch_bytes);
}

void CodePatch::write_patch(std::vector<int16_t> patch_code){
    assert(patch_is_built);
    uint8_t* patch_address_bytes = reinterpret_cast<uint8_t*>(patch_address);
    DWORD prota, protb;
    VirtualProtect(reinterpret_cast<void*>(patch_address), size, PAGE_EXECUTE_READWRITE, &prota);
    for (int i = 0; i < size; i++){
        if (patch_code[i] != -1){
            patch_address_bytes[i] = static_cast<uint8_t>(patch_code[i]);
        };
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
    if (applied){
        write_patch(original_code);
        printf("done\n");
    }else{
        if (patch_is_built){
            printf("wasn't needed.\n");
        }else{
            printf("wasn't built.\n");
        };
    };
    applied = false;
}

bool CodePatch::check_integrity(){
    assert(patch_is_built);
    uint8_t* patch_address_bytes = reinterpret_cast<uint8_t*>(patch_address);
    std::vector<int16_t> comparison_code;

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
    return applied;
}

size_t CodePatch::get_size(){
    assert(patch_is_built);
    return size;
}

bool CodePatch::is_built(){
    return patch_is_built;
}

std::vector<int16_t> CodePatch::get_bytes_from_patch_address(){
    uint8_t* patch_address_bytes = reinterpret_cast<uint8_t*>(patch_address);
    std::vector<int16_t> found_code;
    for (int i = 0; i < size; i++){
        found_code.push_back(patch_address_bytes[i]);
    };
    return found_code;
}

std::vector<int16_t> CodePatch::get_unpatched_bytes(){
    assert(patch_is_built);
    return original_code;
}

std::vector<int16_t> CodePatch::get_patched_bytes(){
    assert(patch_is_built);
    return patched_code;
}

uintptr_t CodePatch::get_return_address(){
    assert(patch_is_built);
    return return_address;
}

uintptr_t get_call_address(intptr_t call_pointer){
    uint8_t* call_bytes = reinterpret_cast<uint8_t*>(call_pointer);
    assert(call_bytes[0] == 0xE8 || call_bytes[0] == 0xE9 || call_bytes[0] == 0x0F);
    if (call_bytes[0] == 0xE8 || call_bytes[0] == 0xE9){
        return (*reinterpret_cast<uintptr_t*>(call_pointer + 1) + call_pointer + 5);
    }else if((call_bytes[0] == 0x0F)){
        return (*reinterpret_cast<uintptr_t*>(call_pointer + 2) + call_pointer + 6);
    }
    return 0;
}

void set_call_address(intptr_t call_pointer, intptr_t point_to){
    DWORD prota, protb;
    uint8_t* call_bytes = reinterpret_cast<uint8_t*>(call_pointer);
    assert(call_bytes[0] == 0xE8 || call_bytes[0] == 0xE9);
    if (call_bytes[0] == 0xE8 || call_bytes[0] == 0xE9){
        VirtualProtect(reinterpret_cast<void*>(call_pointer), 5, PAGE_EXECUTE_READWRITE, &prota);
        *reinterpret_cast<intptr_t*>(call_pointer + 1) = point_to - 5 - call_pointer;
        VirtualProtect(reinterpret_cast<void*>(call_pointer), 5, prota, &protb);
    };

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
