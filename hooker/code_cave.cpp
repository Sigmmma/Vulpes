#include "hooker.hpp"
#define WIN32_MEAN_AND_LEAN
#include "windows.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cassert>

// A little background. The sane way to hook into an event would be to
// overwrite a call to a function that we want to wrap. Problem is, everyone
// is already doing that. And what is absolutely insane is crawling up their
// hooks and working around that.
// So the most sane solution that presented itself is to hook into the start
// of the function we want to wrap. We jump to our own code instead of
// letting it start executing the original code. Then when it is time to
// execute the game code we execute the instructions we overwrote and then
// jump to the rest of the function. The way it returns to our code is
// that we overwrote the original return address with an address right after
// the jump. That area will execute the after part of the wrapper and return
// to the place that originally called the function we are hooking into.

__attribute__((naked))
void null_func(){
    asm ("ret");
}

const uint8_t code_cave_template[] = {
    // save return address.
    0x60,
    0x8b, 0x44, 0x24, 0x20,
    0xe8, 0x00, 0x00, 0x00, 0x00,
    0x61,
    // before function.
    0x60,
    0x54, // Push esp and increment the pushed value by 24
    0x3e, 0x83, 0x04, 0x24, 0x24,
    0xe8, 0x00, 0x00, 0x00, 0x00,
    0x83, 0xc4, 0x04,
    0x84, 0xc0,
    0x74, 0x2F,
    0x61,
    // remove original return address from stack.
    0x83, 0xc4, 0x04,
    // push the address after the next jump onto the stack,
    // so the function will return here and finish.
    0x68, 0x00, 0x00, 0x00, 0x00,
    // execute the portion we overwrote.
    0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
    // jump to the function we're hooking into.
    0xe9, 0x00, 0x00, 0x00, 0x00,
    // make space for a return address.
    0x6a, 0x00,
    // execute after function.
    0x60,
    0x54, // Push esp and increment the pushed value by 24
    0x3e, 0x83, 0x04, 0x24, 0x24,
    0xe8, 0x00, 0x00, 0x00, 0x00,
    0x83, 0xc4, 0x04,
    // get return address and put it into our allocated spot.
    0xe8, 0x00, 0x00, 0x00, 0x00,
    0x89, 0x44, 0x24, 0x20,
    0x61,
    // return to whatever place called the original function.
    0xc3
};

// This is so we can track return addresses for each thread.
// I absolutely do not want to end up in the position of having
// a condition where a function we hooked into gets called twice
// and one of them returns to the wrong call address.

typedef struct {
    uint32_t thread_id;
    uint32_t ret_addr;
    bool filled;
}ThreadReturnAddressPair;

ThreadReturnAddressPair t_pairs[200];

__attribute__((regparm(1)))
void register_return_address(uint32_t return_addr){
    uint32_t thread_id = GetCurrentThreadId();
    int selected_slot = -1;
    for (int i = 0; i<200; i++){
        if (!t_pairs[i].filled && selected_slot < 0){
            selected_slot = i;
        };
        if (thread_id == t_pairs[i].thread_id){
            selected_slot = i;
            break;
        };
    };
    if (selected_slot >= 0){
        t_pairs[selected_slot].thread_id = thread_id;
        t_pairs[selected_slot].ret_addr = return_addr;
        t_pairs[selected_slot].filled = true;
    }else{
        MessageBox(NULL, "Fuck fuck fuuuck, we ran out of thread slots!!! Report to Michelle!!!", "OH FUUUCK", MB_OK);
        exit(0);
    };
}

__attribute__((cdecl))
uint32_t unregister_return_address(){
    uint32_t thread_id = GetCurrentThreadId();
    int selected_slot = -1;
    uint32_t return_addr;
    for (int i = 0; i<200; i++){
        if (t_pairs[i].filled && thread_id == t_pairs[i].thread_id){
            selected_slot = i;
            break;
        };
    };
    if (selected_slot >= 0){
        return_addr = t_pairs[selected_slot].ret_addr;
        t_pairs[selected_slot].thread_id = 0;
        t_pairs[selected_slot].ret_addr = 0;
        t_pairs[selected_slot].filled = false;
    }else{
        MessageBox(NULL, "Fuck fuck fuuuck, we ran out of thread slots!!! Report to Michelle!!!", "OH FUUUCK", MB_OK);
        exit(0);
    };
    return return_addr;
}

static size_t address_of_next_cave = 0;
const size_t SIZE_OF_CODE_CAVE_MEMORY = 16384;
static uint8_t* code_cave_memory;

void init_code_caves(){
    for (int i = 0; i < 200; i++){
        t_pairs[i].thread_id = 0;
        t_pairs[i].ret_addr = 0;
        t_pairs[i].filled = false;
    };
    code_cave_memory = (uint8_t*)VirtualAlloc(NULL, SIZE_OF_CODE_CAVE_MEMORY,
                                              MEM_COMMIT | MEM_RESERVE,
                                              PAGE_EXECUTE_READWRITE);
    address_of_next_cave = (size_t)code_cave_memory;
    if (code_cave_memory == NULL){
        MessageBox(NULL, "Couldn't allocate memory for event hooks. Geez, I only asked for 16kilobytes", "Really!?", MB_OK);
        exit(0);
    };
}

void revert_code_caves(){
    if (code_cave_memory != NULL){
        free(code_cave_memory);
    };
}

intptr_t prepare_code_cave(){
    size_t curr_cave = address_of_next_cave;
    address_of_next_cave += sizeof(code_cave_template);
    if (address_of_next_cave >= ((intptr_t)code_cave_memory + SIZE_OF_CODE_CAVE_MEMORY)){
        MessageBox(NULL, "Ran out of space for hooks.", "Upgrade the hook space.", MB_OK);
        exit(0);
    };

    memcpy((void*)curr_cave, code_cave_template, sizeof(code_cave_template)); // Get default pattern into the space.
    set_call_address(curr_cave+5,    (intptr_t)&register_return_address); // register original return address
    set_call_address(curr_cave+0x4D, (intptr_t)&unregister_return_address); // retrieve original return address
    *(intptr_t*)(curr_cave+0x23)  =  (intptr_t)curr_cave+0x3C; // Return address
    set_call_address(curr_cave+0x12, (intptr_t)&null_func); // before function placeholder
    set_call_address(curr_cave+0x45, (intptr_t)&null_func); // after function placeholder
    set_call_address(curr_cave+0x37, (intptr_t)&null_func); // jump placeholder
    return curr_cave;
}
/*
CodeCave::CodeCave(const char* h_name, uintptr_t p_address, size_t p_size, void* before, void* after){
    code_patch = CodePatch(h_name);
    name = h_name;
    before_func = (intptr_t)before;
    after_func = (intptr_t)after;
}
*/
bool CodeCave::build(intptr_t p_address){
    if (!code_patch.is_built()){
        printf("Building CodeCave %s...", name);
        // We only have 16 bytes allocated for instruction copies.
        assert(patch_size <= 16);
        // If we got passed a patch address, use it.
        if (p_address) patch_address = p_address;
        // If the patch address == 0 we're likely just using a signature.
        if (!patch_address) patch_address = sig.address() + patch_offset;
        // Fail condition.
        if (patch_address - patch_offset <= 0){
            printf("failed.");
            return false;
        };
        // Allocate and pregenerate a cave.
        cave_address = (intptr_t)prepare_code_cave();
        // Determine if our hook is placed on a call instruction.
        // Build the patch accordingly.
        bool is_call_hook = false;
        if (*(uint8_t*)patch_address == CALL_BYTE){
            is_call_hook = true;
            code_patch = CodePatch(name,patch_address,patch_size,CALL_PATCH,cave_address);
        }else{
            code_patch = CodePatch(name,patch_address,patch_size,JMP_PATCH,cave_address);
        };

        if (code_patch.build()){
            // Write the before and after calls to the appropriate spots in the cave.
            set_call_address(cave_address+0x12, before_func);
            set_call_address(cave_address+0x45, after_func);
            // Make a copy of the original code we're overwriting.
            uint8_t* original_code_cpy = (uint8_t*)(cave_address+0x27);
            std::vector<int16_t> original_code = code_patch.unpatched_bytes();
            // If we're not a call hook then we can in most cases (except for jumps) just copy the code.
            if (!is_call_hook){
                for (int i = 0; i<original_code.size(); i++){
                    original_code_cpy[i] = (uint8_t)original_code[i];
                };
                original_code_cpy[original_code.size()] = JMP_BYTE;
                set_call_address((intptr_t)&original_code_cpy[original_code.size()], code_patch.return_address());
            }else{ // If it is a call we'll need to regenerate the offset to be correct for our cave.
                original_code_cpy[0] = JMP_BYTE;
                set_call_address((intptr_t)&original_code_cpy[0], get_call_address(patch_address));
            };
            printf("done.\n");
        };
    };
    if (!code_patch.is_built()){
        printf("failed.");
        return false;
    };
    return true;
}

void CodeCave::apply(){code_patch.apply();}
void CodeCave::revert(){code_patch.revert();}
CodePatch* CodeCave::patch(){return &code_patch;}
