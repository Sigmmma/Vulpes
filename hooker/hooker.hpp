/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#pragma once

#include <vector>

#include "resource.hpp"

// Macro for CodeSignature, this is to make it so we don't have to repeat ourselves too much.
#define Signature(required, name, ...) CodeSignature name(required, #name, 0, 0, std::vector<int16_t>(__VA_ARGS__))
#define SignatureBounded(required, name, lowest_address, highest_address, ...) CodeSignature name(required, #name, lowest_address, highest_address, std::vector<int16_t>(__VA_ARGS__))

// A class for finding code inside of the Halo executable during runtime.
class CodeSignature {
public:
    // Initializers
    // If lowest search address and/or highest_search address are 0 they default to the bounds above.
    CodeSignature(bool required,
                  const char* d_name,
                  uintptr_t lowest_search_address,
                  uintptr_t highest_search_address,
                  std::vector<int16_t> signature);

    // Returns the address and does a search if it hasn't already.
    // Returns 0 if address is not found.
    // Returns previously found value if it has already searched.
    uintptr_t address(uintptr_t start_address = 0, uintptr_t end_address = 0);
    uintptr_t address(bool recalculate);

private:
    uintptr_t found_address = 0;
    std::vector<int16_t> sig;
    uintptr_t lowest_allowed;
    uintptr_t highest_allowed;
    bool imperative;
    bool already_tried = false;
    const char* name;
};

enum PatchTypes {
    NOP_PATCH,  // NOPs out the code so it does nothing.
    CALL_PATCH, // Makes a function call to redirect_to.
    JMP_PATCH,  // Makes a jump to redirect_to.
    JA_PATCH,  // Makes a conditional jump to redirect_to.
    SKIP_PATCH,  // Puts a jmp at the start of the patch area which jumps to the end of the patch area. Pads the rest with NOPs.
    INT_PATCH,
    MANUAL_PATCH // Requires you to pass your own bytes to write at the given area.
};

// A macro so we don't have to fill in the name twice.
#define Patch(name, ...) CodePatch name(#name, __VA_ARGS__)
// A class for patching code.
class CodePatch {
private:
    void setup_internal(void* content, size_t c_size);
public:
    ////// Initlializers.
    // Dynamic initializers.
    // Can be used with any of the PatchTypes from the enum above.
    template<typename T>
    CodePatch(const char* d_name, CodeSignature& p_sig, int p_sig_offset,
              size_t p_size, PatchTypes p_type, T content) CPTEMPLINIT1;
    template<typename T>
    CodePatch(const char* d_name, intptr_t p_address,
              size_t p_size, PatchTypes p_type, T content) CPTEMPLINIT2;
    // MANUAL_PATCH initializers.
    // These are used for self specifying patch bytes in vectors.
    // Similarily to CodeSignature you can use -1 in the vector for wildcards
    // that do not have their data changed.
    CodePatch(const char* d_name, CodeSignature& p_sig, int p_sig_offset,
              std::vector<int16_t> patch_bytes);
    CodePatch(const char* d_name, intptr_t p_address,
              std::vector<int16_t> patch_bytes);
    intptr_t address();
    ////// Runtime functions.
    // Builds the patch at based on the data found at the current address.
    // If not address is put in it will use the address passed earlier,
    // or use a code signature to try and find an address.
    // Returns true if success, false at fail, true if already built.
    // This does not apply the patch.
    bool build(intptr_t p_address = 0);
    // Applies the patch.
    void apply();
    // Reverts the code to the original bytes.
    void revert();
    // Returns the address that comes right after the code patch.
    // For use in return jumps found in hooks.
    uintptr_t return_address();

    ////// Mostly debug functions after this point.
    // Checks if the code was changed since the last time we touched it.
    // Returns true if our patched/the unpatched code is intact. False if not.
    bool integrity();
    // See if the patch is supposed to be patch_applied.
    bool applied();
    // Returns the patch size.
    size_t size();
    // Returns wether or not the patch has been built.
    bool is_built();
    // Returns the bytes that are currently at the patch address.
    std::vector<int16_t> bytes_at_patch_address();
    // Returns the bytes that were at the address at the time the patch was built.
    std::vector<int16_t> unpatched_bytes();
    // Returns the bytes that the patch would put at the patch address.
    std::vector<int16_t> patched_bytes();
private:
    CodeSignature sig = CodeSignature(false,"",0,0,{});
    int offset = 0;
    uintptr_t patch_address = 0;
    uintptr_t redirect_address = 0;
    std::vector<int16_t> original_code;
    std::vector<int16_t> patched_code;
    size_t patch_size;
    bool patch_built = false;
    bool patch_applied = false;
    PatchTypes type;
    const char* name;
    void write_patch(std::vector<int16_t> patch_code);
};

// Gets the direct pointer to whatever the instruction at this address CALLs or JUMPs to.
uintptr_t get_call_address(intptr_t call_pointer);
void      set_call_address(intptr_t call_pointer, intptr_t point_to);

template<typename T>
uintptr_t get_call_address(T call_pointer) {
    return get_call_address(*reinterpret_cast<intptr_t*>(&call_pointer));
}

template<typename T, typename T2>
void set_call_address(T call_pointer, T2 point_to) {
    set_call_address(*reinterpret_cast<intptr_t*>(&call_pointer),
                     *reinterpret_cast<intptr_t*>(&point_to));
}

// These are initialization functions, these should rarely actually be called.
uintptr_t get_lowest_permitted_address();
uintptr_t get_highest_permitted_address();
void set_lowest_permitted_address(uintptr_t new_address);
void set_highest_permitted_address(uintptr_t new_address);
