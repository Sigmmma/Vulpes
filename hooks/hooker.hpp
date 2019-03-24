#pragma once

#include <cstdint>
#include <cstddef>
#include <stdbool.h>

#include <vector>

uintptr_t LOWEST_PERMITTED_ADDRESS  = 0x400000; // Everything under this address is part of the stack.
uintptr_t HIGHEST_PERMITTED_ADDRESS = 0x700000; // The address I have determined after which there is no code anymore on the executable.


// A class for finding code inside of the Halo executable during runtime.
class CodeSignature {
    uintptr_t address = 0;
    std::vector<int16_t> sig;
    uintptr_t lowest_allowed = LOWEST_PERMITTED_ADDRESS;
    uintptr_t highest_allowed = HIGHEST_PERMITTED_ADDRESS;
    bool imperative = true;
    bool already_tried = false;
public:
    // Initializers
    CodeSignature(bool required, std::vector<int16_t> signature){
        imperative = required; sig = signature; }
    CodeSignature(bool required, uintptr_t lowest_search_address, uintptr_t highest_search_address, std::vector<int16_t> signature){
        imperative = required; lowest_allowed = lowest_search_address; highest_allowed = highest_search_address; sig = signature;}

    // Returns the address and does a search if it hasn't already.
    // Returns 0 if address is not found.
    uintptr_t get_address();
    uintptr_t get_address(bool recalculate);
};

enum PatchTypes {
    NOP_PATCH,  // NOPs out the code so it does nothing.
    CALL_PATCH, // Makes a function call to redirect_to.
    JMP_PATCH,  // Makes a jump to redirect_to.
    SKIP_PATCH  // Puts a jmp at the start of the patch area which jumps to the end of the patch area. Pads the rest with NOPs.
};

// A class for patching code.
class CodePatch {
public:
    // Initlializers.
    CodePatch(uintptr_t p_address, size_t p_size, PatchTypes p_type, uintptr_t redirect_to);

    ////// Main functions.

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
    std::vector<uint8_t> get_bytes_from_patch_address();
    // Returns the bytes that normally should be at the patch address.
    std::vector<uint8_t> get_unpatched_bytes();
    // Returns the bytes that the patch would put at the patch address.
    std::vector<uint8_t> get_patched_bytes();
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
    std::vector<uint8_t> original_code;
    std::vector<uint8_t> patched_code;
    size_t size;
    bool patch_is_built;
    bool applied;
    PatchTypes type;

    void write_patch(std::vector<uint8_t> patch_code);
};
