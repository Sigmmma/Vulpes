#pragma once

#include <cstdint>
#include <vector>

// Macro for CodeSignature, this is to make it so we don't have to repeat ourselves too much.
#define Signature(required, name, ...) CodeSignature name(required, #name, 0, 0, std::vector<int16_t>(__VA_ARGS__))
#define SignatureBounded(required, name, lowest_address, highest_address, ...) CodeSignature name(required, #name, lowest_address, highest_address, std::vector<int16_t>(__VA_ARGS__))

// A class for finding code inside of the Halo executable during runtime.
class CodeSignature {
    uintptr_t address = 0;
    std::vector<int16_t> sig;
    uintptr_t lowest_allowed;
    uintptr_t highest_allowed;
    bool imperative;
    bool already_tried = false;
    const char* name;
public:
    // Initializers
    // If lowest search address and/or highest_search address are 0 they default to the bounds above.
    CodeSignature(bool required, const char* d_name, uintptr_t lowest_search_address, uintptr_t highest_search_address, std::vector<int16_t> signature){
        imperative = required; lowest_allowed = lowest_search_address; highest_allowed = highest_search_address; sig = signature; name = d_name;}

    // Returns the address and does a search if it hasn't already.
    // Returns 0 if address is not found.
    uintptr_t get_address();
    uintptr_t get_address(bool recalculate);
};

enum PatchTypes {
    NOP_PATCH,  // NOPs out the code so it does nothing.
    CALL_PATCH, // Makes a function call to redirect_to.
    JMP_PATCH,  // Makes a jump to redirect_to.
    SKIP_PATCH,  // Puts a jmp at the start of the patch area which jumps to the end of the patch area. Pads the rest with NOPs.
    MANUAL_PATCH // Requires you to pass your own bytes to write at the given area.
};

// A macro so we don't have to fill in the name twice.
#define Patch(name) CodePatch name(#name)

// A class for patching code.
class CodePatch {
public:
    // Initlializers.
    CodePatch(const char * d_name){name = d_name;};

    ////// Main functions.
    void build(uintptr_t p_address, size_t p_size, PatchTypes p_type, uintptr_t redirect_to);
    // Allows you to specify your own patch bytes,
    // with -1 in the place of bytes you don't want changed by the patch.
    void build_manual(uintptr_t p_address, std::vector<int16_t> patch_bytes);
    // Applies the patch.
    void apply();
    // Reverts the code to the original bytes.
    void revert();
    // Returns the address that comes right after the code patch. For use in return jumps found in hooks.
    uintptr_t get_return_address();

    ////// Mostly debug functions after this point.

    // Checks if the code was changed since the last time we touched it.
    // Used for patches that might be applied later, to see if another mod touched it.
    // Also used to check if another mod overwrote our patch. (If we didn't crash and burn yet at that point.)
    bool check_integrity();
    // See if the patch is supposed to be applied.
    bool is_applied();
    // Returns the patch size.
    size_t get_size();
    // Returns the bytes that are currently at the patch address.
    std::vector<int16_t> get_bytes_from_patch_address();
    // Returns the bytes that were at the address at the time the patch was built.
    std::vector<int16_t> get_unpatched_bytes();
    // Returns the bytes that the patch would put at the patch address.
    std::vector<int16_t> get_patched_bytes();
private:
    enum InstructionBytes : uint8_t {
        NOP_BYTE  = 0x90,
        CALL_BYTE = 0xE8,
        JMP_BYTE  = 0xE9,
        JMP_SMALL_BYTE = 0xEB
    };
    uintptr_t patch_address;
    uintptr_t redirect_address;
    uintptr_t return_address;
    std::vector<int16_t> original_code;
    std::vector<int16_t> patched_code;
    size_t size;
    bool patch_is_built;
    bool applied;
    PatchTypes type;
    const char* name;

    void write_patch(std::vector<int16_t> patch_code);
};

uintptr_t get_call_address(uintptr_t call_pointer);


// These are initialization functions, these should rarely actually be called.
uintptr_t get_lowest_permitted_address();
uintptr_t get_highest_permitted_address();
void set_lowest_permitted_address(uintptr_t new_address);
void set_highest_permitted_address(uintptr_t new_address);
