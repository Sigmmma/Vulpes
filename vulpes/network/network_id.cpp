/*
 * This file is part of Vulpes, an extension of Halo Custom Edition's capabilities.
 * Copyright (C) 2019-2020 gbMichelle (Michelle van der Graaf)
 *
 * Vulpes is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, version 3.
 *
 * Vulpes is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * long with Vulpes.  If not, see <https://www.gnu.org/licenses/agpl-3.0.en.html>
 */

#include <vulpes/memory/signatures/signatures.hpp>

#include "network_id.hpp"

static uintptr_t message_delta_object_index;
static uintptr_t func_server_register_network_index;
static uintptr_t func_client_register_network_index_from_remote;
static uintptr_t func_unregister_network_index;

// Pointer to a pointer because the game might change the actual pointer
// So we want to read what is in the game's pointer to make sure we're safe.
SyncedObjectHeader** synced_objects_header = NULL;

int32_t server_register_network_index(MemRef object) {
    SyncedObjectHeader* synced_objects = *synced_objects_header;
    if (synced_objects->count >= synced_objects->max_count) {
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

void register_network_index_from_remote(int32_t network_id, MemRef object) {
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

void unregister_network_index(MemRef object) {
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

MemRef get_object_from_network_index(int32_t network_id) {
    SyncedObjectHeader* synced_objects = *synced_objects_header;
    return synced_objects->translation_index[network_id];
}

int32_t get_network_id_from_object(MemRef object) {
    SyncedObjectHeader* synced_objects = *synced_objects_header;
    for (int i=1; i < synced_objects->max_count; i++) {
        if (synced_objects->translation_index[i].raw == object.raw) {
            return i;
        }
    }
    return -1;
}

void init_network_id() {
    synced_objects_header = *reinterpret_cast<SyncedObjectHeader***>(
        sig_network_translation_table());
    func_server_register_network_index =
        sig_func_server_register_network_index();
    func_client_register_network_index_from_remote =
        sig_func_client_register_network_index_from_remote();
    func_unregister_network_index =
        sig_func_unregister_network_index();
}

SyncedObjectHeader* synced_objects() {
    return *synced_objects_header;
}
