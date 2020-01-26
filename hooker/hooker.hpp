/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#pragma once

#include <cstdint>
#include <vector>

enum InstructionBytes : uint8_t {
    NOP_BYTE  = 0x90,
    CALL_BYTE = 0xE8,
    JMP_BYTE  = 0xE9,
    JMP_SMALL_BYTE = 0xEB,

    PUSHFD_BYTE = 0x9C,
    PUSHAD_BYTE = 0x60,
    POPFD_BYTE  = 0x91,
    POPAD_BYTE  = 0x61,
    PUSH_EDI_BYTE = 0x57,

    // 2 byte conditional jumps
    CONDJ_BYTE = 0x0F, // first byte

    JA_BYTE    = 0x87
};

class LiteSignature {
public:
    const char* name;
    const size_t size;
    const int16_t bytes[];

    uintptr_t search(
        uintptr_t start_address = 0, uintptr_t end_address = 0);
    std::vector<uintptr_t> search_multiple(
        uintptr_t start_address = 0, uintptr_t end_address = 0);
};

enum PatchTypes {
    NOP_PATCH,   // NOPs out the code so it does nothing.
    CALL_PATCH,  // Makes a function call to redirect_to.
    JMP_PATCH,   // Makes a jump to redirect_to.
    JA_PATCH,    // Makes a conditional jump to redirect_to.
    SKIP_PATCH,  // Puts a jmp at the start of the patch area which jumps
                 // to the end of the patch area. Pads the rest with NOPs.
    INT_PATCH,
    MANUAL_PATCH // Requires you to pass your own bytes.
};

// A macro so we don't have to fill in the name twice.
#define Patch(name, ...) CodePatch name(#name, __VA_ARGS__)
// A class for patching code.
class CodePatch {
public:
    template<typename T>
    CodePatch(const char* d_name,
              uintptr_t p_address,
              size_t p_size,
              PatchTypes p_type,
              T content) {
        name = d_name;
        patch_address = p_address;
        patch_size = p_size;
        type = p_type;

        switch(type) {
            case JA_PATCH :
            case JMP_PATCH :
            case CALL_PATCH :
                // Every 4 byte type is technically valid for this type of
                // patch. Every time you define a function with slightly
                // different arguments that is seen as a completely different
                // type. So, this is the sane alternative to converting in the
                // files that initialize the patches.
                redirect_address = *reinterpret_cast<const uintptr_t*>(&content);
                break;
            case INT_PATCH :
                // Int patches are arbitrary size and just write an integer
                // to the patch location. We can just write these byte by byte
                // so to save on code we do so.
                type = MANUAL_PATCH;
                /* Use MANUAL_PATCH setup */
            case MANUAL_PATCH :
                // Manual patches are just arrays of bytes.
                // We copy those for safety.
                for (int i=0; i<sizeof(T);i++) {
                    patched_code.push_back(
                        reinterpret_cast<const uint8_t*>(&content)[i]
                    );
                }
                break;
            default:
                // Every other patch type is covered by the part above this
                // switch.
                break;
        }
    }
    // MANUAL_PATCH initializers.
    // These are used for self specifying patch bytes in vectors.
    // Similarily to CodeSignature you can use -1 in the vector for wildcards
    // that do not have their data changed.
    CodePatch(const char* d_name,
              uintptr_t p_address,
              std::vector<int16_t> patch_bytes);


    ////// Runtime functions.

    // Gets the address the patch intends to write to.
    uintptr_t address();
    // Builds the patch at based on the data found at the current address.
    // If no address is put in it will use the address passed earlier,
    // or use a code signature to try and find an address.
    // Returns true if success, false at fail, true if already built.
    // This does not apply the patch.
    bool build(uintptr_t p_address = 0);
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
