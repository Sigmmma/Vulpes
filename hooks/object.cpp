#include <cstdint>
#include "hooker.hpp"
#include "../memory/object.hpp"

Signature(true, sig_object_full_id_collection,
    {0xE8, -1, -1, -1, -1, 0x8B, 0xD8, 0x83, 0xC4, 0x08, 0x83, 0xFB, 0xFF, 0x0F, 0x84, -1, -1, -1, -1, 0x8B, 0x4E, 0x34});
uintptr_t patch_adress = sig_object_full_memref_id_collection.get_address();

uintptr_t func_object_header_new = get_call_address(patch_adress);
uintptr_t array_object_full_ids = reinterpret_cast<uintptr_t>(object_full_ids);

CodePatch patch_object_full_id_collection(patch_adress, 5, CALL_PATCH, func_object_header_new);

__attribute__((naked))
uint32_t object_header_new_wrapper(){
    asm(
        // Call original code to get the return value.
        "call %[object_header_new];\n"
        // Protect these registers.
        "push edi;\n"
        "push esi;\n"
        // Isolate local object id to get how far into our array we need to put the full id.
        "mov edi, eax;\n"
        "and edi, 0xFFFF\n"
        // Set esi to the exact address that the array entry should be at.
        "mov esi, %[object_index_list];\n"
        "lea esi, [esi+4*edi]\n"
        // Write the full index to the array.
        "mov [esi], eax;\n"
        // Get these registers back
        "pop esi;\n"
        "pop edi;\n"
        // End of hook.
        "ret;\n"
        : [object_header_new] "+m" (func_object_header_new)
        : [object_index_list] "m" (array_object_full_ids)
    );
}

// get sig of client address 56BDF3 to fix grenades dissapearing.

void init_object_full_id_collection_hook(){
    patch_object_full_id_collection.apply();
}
void revert_object_full_id_collection_hook(){
    patch_object_full_id_collection.revert();
}

void init_object_hooks(){
    init_object_full_id_collection_hook();
}

void revert_object_hooks(){
    revert_object_full_id_collection_hook();
}
