#include "hooker.hpp"
#define WIN32_MEAN_AND_LEAN
#include "windows.h"
#include <cstdlib>
#include <cstdio>
#include <string.h>

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
    0xe8, 0x00, 0x00, 0x00, 0x00, // call write offset 5
    0x61,
    // before function.
    0x60,
    0xe8, 0x00, 0x00, 0x00, 0x00, // call write offset 0xC
    0x61,
    // remove original return address from stack.
    0x83, 0xc4, 0x04,
    // push the address after the next jump onto the stack,
    // so the function will return here and finish.
    0x68, 0x00, 0x00, 0x00, 0x00, // return write offset 0x16
    // execute the portion we overwrote.
    0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, // original code write offset 0x1A
    // jump to the function we're hooking into.
    0xe9, 0x00, 0x00, 0x00, 0x00, // execute original func write offset 0x2A
    // make space for a return address.
    0x6a, 0x00,
    // execute after function.
    0x60,
    0xe8, 0x00, 0x00, 0x00, 0x00, // call write offset 0x32
    // get return address and put it into our allocated spot.
    0xe8, 0x00, 0x00, 0x00, 0x00, // call write offset 0x37
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

void init_event_hooker(){
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

void revert_event_hooker(){
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
    set_call_address(curr_cave+0x5,  (intptr_t)&register_return_address); // register original return address
    set_call_address(curr_cave+0x37, (intptr_t)&unregister_return_address); // retrieve original return address
    *(intptr_t*)(curr_cave+0x16)  =  (intptr_t)curr_cave+0x2F; // Return address
    set_call_address(curr_cave+0xC,  (intptr_t)&null_func); // before function placeholder
    set_call_address(curr_cave+0x32, (intptr_t)&null_func); // after function placeholder
    set_call_address(curr_cave+0x2A, (intptr_t)&null_func); // jump placeholder
    return curr_cave;
}

EventHook::EventHook(const char* h_name, void* before, void* after){
    code_patch = CodePatch(h_name);
    name = h_name;
    before_func = (intptr_t)before;
    after_func = (intptr_t)after;
}

void EventHook::build(uintptr_t p_address, size_t p_size){
    printf("Building EventHook %s...", name);
    cave_address = (intptr_t)prepare_code_cave();
    code_patch.build(p_address, p_size, JMP_PATCH, cave_address);
    set_call_address(cave_address+0xC,  before_func);
    set_call_address(cave_address+0x32, after_func);
    set_call_address(cave_address+0x2A, code_patch.get_return_address());
    uint8_t* original_code_cpy = (uint8_t*)cave_address+0x1A;
    std::vector<int16_t> original_code = code_patch.get_unpatched_bytes();
    for (int i = 0; i<original_code.size(); i++){
        original_code_cpy[i] = (uint8_t)original_code[i];
    };
    printf("done.");
}

void EventHook::apply(){
    code_patch.apply();
}

void EventHook::revert(){
    code_patch.revert();
}
