#include "definition.hpp"
#include "enums.hpp"
#include "../../hooks/hooker.hpp"
#include "vulpes_message.hpp"

#include <cstdint>
#define WIN32_MEAN_AND_LEAN
#include <windows.h>

//mdpi encode

Signature(true, sig_mdpi_encode,
    {0x81, 0xEC, 0xA4, 0x00, 0x00, 0x00, 0x53, 0x55, 0x8B, 0xAC, 0x24, 0xBC, 0x00, 0x00, 0x00, 0x33, 0xDB, 0x56});

static uintptr_t func_mdpi_encode_ptr;
static uintptr_t old_list_ptr;
static uintptr_t old_list_end_ptr;

static uintptr_t message_definitions[0x40];
static uintptr_t new_list_end_ptr = reinterpret_cast<uintptr_t>(&message_definitions) + NEW_MESSAGE_TYPE_COUNT * 4;

// TODO:
// Store the original address of the definitions list
// Store a list of pointers to all locations that were changed.
// Make a revert function using the two points above.

void init_new_definitions(){
    message_definitions[VULPES_MSG] = get_vulpes_message_definition();
    func_mdpi_encode_ptr = sig_mdpi_encode.get_address();
    old_list_ptr = *reinterpret_cast<uintptr_t*>(func_mdpi_encode_ptr+0x30);
    old_list_end_ptr = old_list_ptr + DEFAULT_MESSAGE_TYPE_COUNT * 4;
    uintptr_t* old_messages_array = reinterpret_cast<uintptr_t*>(old_list_ptr);

    for (int i=0; i<DEFAULT_MESSAGE_TYPE_COUNT; i++){
        message_definitions[i] = old_messages_array[i];
    };
    std::vector<int16_t> ptr_bytes;
    std::vector<int16_t> end_ptr_bytes;
    for (int i=0; i<4; i++){
        ptr_bytes.push_back(reinterpret_cast<uint8_t*>(&old_list_ptr)[i]);
        end_ptr_bytes.push_back(reinterpret_cast<uint8_t*>(&old_list_end_ptr)[i]);
    };
    CodeSignature sig_mdp_lists(false, "sig_mdp_lists",0,0, ptr_bytes);
    CodeSignature sig_mdp_list_ends(false, "sig_mdp_list_ends",0,0, end_ptr_bytes);

    uintptr_t addr = sig_mdp_lists.get_address();
    while (addr != 0){
        DWORD prota, protb;
        VirtualProtect((void*)addr, 4, PAGE_EXECUTE_READWRITE, &prota);
        *reinterpret_cast<uintptr_t*>(addr) = reinterpret_cast<uintptr_t>(&message_definitions);
        VirtualProtect((void*)addr, 4, prota, &protb);
        addr = sig_mdp_lists.get_address(true);
    };
    addr = sig_mdp_list_ends.get_address();
    while (addr != 0){
        DWORD prota, protb;
        VirtualProtect((void*)addr, 4, PAGE_EXECUTE_READWRITE, &prota);
        *reinterpret_cast<uintptr_t*>(addr) = new_list_end_ptr;
        VirtualProtect((void*)addr, 4, prota, &protb);
        addr = sig_mdp_list_ends.get_address(true);
    };
}
