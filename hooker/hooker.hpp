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
    uintptr_t address();
    uintptr_t address(bool recalculate);
    uintptr_t address(uintptr_t start_address);
    uintptr_t address(uintptr_t start_address, uintptr_t end_address);
private:
    uintptr_t address = 0;
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
#define Patch(name) CodePatch name(#name)
#define PatchNew(name, ...) CodePatch name(#name, __VA_ARGS__)
// A class for patching code.
class CodePatch {
private:
    void setup_internal(size_t p_size, PatchTypes p_type, void* content, size_t c_size);
public:
    // Initlializers.
    template<typename T>
    CodePatch(const char* d_name,
              CodeSignature& p_sig, int p_sig_offset,
              size_t p_size, PatchTypes p_type, T content){
        name = d_name; sig = p_sig; offset = p_sig_offset;
        setup_internal(p_size, p_type, reinterpret_cast<void*>(&content), sizeof(T));
    };
    template<typename T>
    CodePatch(const char* d_name,
              intptr_t p_address,
              size_t p_size, PatchTypes p_type, T content){
        name = d_name; patch_address = p_address;
        setup_internal(p_size, p_type, reinterpret_cast<void*>(&content), sizeof(T));
    };
    CodePatch(const char* d_name,
              CodeSignature& p_sig, int p_sig_offset,
              std::vector<int16_t>& patch_bytes){
        name = d_name; sig = p_sig; offset = p_sig_offset;
        size = patch_bytes.size(); type = MANUAL_PATCH;
        patched_code = patch_bytes;
    };
    CodePatch(const char* d_name,
              intptr_t p_address,
              std::vector<int16_t>& patch_bytes){
        name = d_name; patch_address = p_address;
        size = patch_bytes.size(); type = MANUAL_PATCH;
        patched_code = patch_bytes;
    };
    CodePatch(const char* d_name);
    ////// Main functions.
    bool build(intptr_t p_address = 0);
    void build_old(uintptr_t p_address, size_t p_size, PatchTypes p_type, uintptr_t redirect_to);
    // Allows you to specify your own patch bytes,
    // with -1 in the place of bytes you don't want changed by the patch.
    void build_manual(uintptr_t p_address, std::vector<int16_t> patch_bytes);
    // A patch type that writes an int to a location
    void build_int(uintptr_t p_address, uint32_t patch_int);
    void build_int16(uintptr_t p_address, uint16_t patch_int);
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
    // Returns wether or not the patch has been built.
    bool is_built();
    // Returns the bytes that are currently at the patch address.
    std::vector<int16_t> get_bytes_from_patch_address();
    // Returns the bytes that were at the address at the time the patch was built.
    std::vector<int16_t> get_unpatched_bytes();
    // Returns the bytes that the patch would put at the patch address.
    std::vector<int16_t> get_patched_bytes();
private:
    CodeSignature sig = CodeSignature(false,"",0,0,{});
    int offset = 0;
    uintptr_t patch_address = 0;
    uintptr_t redirect_address = 0;
    uintptr_t return_address = 0;
    std::vector<int16_t> original_code;
    std::vector<int16_t> patched_code;
    size_t size;
    bool patch_is_built = false;
    bool applied = false;
    PatchTypes type;
    const char* name;
    void write_patch(std::vector<int16_t> patch_code);
};

#define Cave(name, before, after) CodeCave name(#name, before, after)

class CodeCave {
public:
    CodeCave(const char* h_name, void* before, void* after);
    void build_old(uintptr_t p_address, size_t p_size);
    void apply();
    void revert();

private:
    intptr_t before_func;
    intptr_t func_continue;
    intptr_t after_func;
    intptr_t cave_address;
    const char* name;
    CodePatch code_patch = CodePatch("");
};

// Gets the direct pointer to whatever the instruction at this address CALLs or JUMPs to.
uintptr_t get_call_address(intptr_t call_pointer);
void      set_call_address(intptr_t call_pointer, intptr_t point_to);


// These are initialization functions, these should rarely actually be called.
uintptr_t get_lowest_permitted_address();
uintptr_t get_highest_permitted_address();
void set_lowest_permitted_address(uintptr_t new_address);
void set_highest_permitted_address(uintptr_t new_address);
void init_code_caves();
void revert_code_caves();
