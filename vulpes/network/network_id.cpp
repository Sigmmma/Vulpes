/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include "network_id.hpp"
#include "../../hooker/hooker.hpp"

Signature(true, sig_func_server_register_network_index, {0x55, 0x56, 0x8B, 0x70, 0x58, 0x8A, 0x46, 0x0C, 0x8D, 0x6E, 0x0C, 0x57, 0x83, 0xCF, 0xFF, 0x3C, 0x01, 0x75});

Signature(true, sig_func_client_register_network_index_from_remote, {0x8B, 0x40, 0x58, 0x8B, 0x50, 0x28, 0x53, 0x55, 0x8B, 0x6C, 0x24, 0x0C, 0x57, 0x8D, 0x3C, 0xAA, 0x8B, 0x17, 0x32, 0xDB, 0x83, 0xFA, 0xFF, 0x75});

Signature(true, sig_func_unregister_network_index, {0x53, 0x57, 0x8B, 0x78, 0x58, 0x8A, 0x47, 0x0C, 0x8D, 0x4F, 0x0C, 0x32, 0xDB, 0x3C, 0x01, 0x75});

Signature(true, sig_translation_table_ptr, {-1, -1, -1, -1, 0x8B, 0x52, 0x28, 0x8B, 0x34, 0x8A});

uintptr_t message_delta_object_index;
uintptr_t func_server_register_network_index;
uintptr_t func_client_register_network_index_from_remote;
uintptr_t func_unregister_network_index;

// Pointer to a pointer because the game might change the actual pointer
// So we want to read what is in the game's pointer to make sure we're safe.
SyncedObjectHeader** synced_objects_header = NULL;

int32_t server_register_network_index(MemRef object){
    SyncedObjectHeader* synced_objects = *synced_objects_header;
    if (synced_objects->count >= synced_objects->max_count){
        // Just don't even try if we're at max capacity.
        return -1;
    }
    int32_t network_id;
    asm (
        // Put arguments where Halo's custom calling convention expects them.
        "mov eax, %[synced_objects];"
        "push %[local_object_id];" // esp - 4
        "call %[server_register_network_index];"
        "mov %[network_id], eax;" // Get return value.
        "add esp, 4;" // Sync stack to before the push
        :
        : [synced_objects] "m" (synced_objects),
          [network_id] "m" (network_id),
          [local_object_id] "m" (object.raw),
          [server_register_network_index] "m" (func_server_register_network_index)
    );
    return network_id;
}

void register_network_index_from_remote(int32_t network_id, MemRef object){
    SyncedObjectHeader* synced_objects = *synced_objects_header;
    asm (
        // Put arguments where Halo's custom calling convention expects them.
        "mov eax, %[synced_objects];"
        "mov ecx, %[local_object_id];"
        "push %[network_id];" // esp - 4
        "call %[client_register_network_index_from_remote];"
        "add esp, 4;" // Sync stack to before the push
        :
        : [synced_objects] "m" (synced_objects),
          [local_object_id] "m" (object.raw),
          [network_id] "m" (network_id),
          [client_register_network_index_from_remote] "m" (func_client_register_network_index_from_remote)
    );
}

void unregister_network_index(MemRef object){
    SyncedObjectHeader* synced_objects = *synced_objects_header;
    asm (
        // Put arguments where Halo's custom calling convention expects them.
        "mov edi, %[synced_objects];"
        "mov esi, %[local_object_id];"
        "call unregister_network_index_prologue;"
        // When the call above is done the code will end up here.
        "jmp after;" // Do this jump to avoid an
                     // ugly repeat ending in a fucked EIP

        // Creating a prologue here because we're starting the function
        // differently than intended.
        // This is essentially a partial function inside of our function.
    "unregister_network_index_prologue:"
        "push ebx;"
        "push edi;"
        "jmp %[unregister_network_index];"

    "after:"
        :
        : [synced_objects] "m" (synced_objects),
          [local_object_id] "m" (object.raw),
          [unregister_network_index] "m" (func_unregister_network_index)
    );
}

MemRef get_object_from_network_index(int32_t network_id){
    SyncedObjectHeader* synced_objects = *synced_objects_header;
    return synced_objects->translation_index[network_id];
}

int32_t get_network_id_from_object(MemRef object){
    SyncedObjectHeader* synced_objects = *synced_objects_header;
    for (int i=1; i < synced_objects->max_count; i++){
        if (synced_objects->translation_index[i].raw == object.raw){
            return i;
        }
    }
    return -1;
}

void init_network_id(){
    // *** - * = **
    /* If we dereference what the signature address comes up with we end up at
     * some code that has a reference to the pointer to the thing that we want.
     * If we store that it means we have a pointer/reference to the
     * pointer the game uses which allows us to always be able to validly check
     * where the game is looking for this struct by reading what is in its ptr.
     */
    synced_objects_header = *reinterpret_cast<SyncedObjectHeader***>(sig_translation_table_ptr.address());
    // +3 because if we enter the function on the second instruction
    // we only need to supply the pointer to the synced_object_header.
    func_server_register_network_index = sig_func_server_register_network_index.address() + 3;
    func_client_register_network_index_from_remote = sig_func_client_register_network_index_from_remote.address() + 3;
    // +5 Here for the same reason.
    func_unregister_network_index = sig_func_unregister_network_index.address() + 5;
}

SyncedObjectHeader* synced_objects(){
    return *synced_objects_header;
}
