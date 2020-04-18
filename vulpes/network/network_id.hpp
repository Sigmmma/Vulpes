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

#pragma once

#include <vulpes/memory/types.hpp>

////// These are functions for using the existing object syncing infrastructure.
////// I really have to stress that we shouldn't use these too much if we start
////// syncronizing things that normally have no network ids.
////// There is only 511 network id slots, and when figuring this all out
////// I have seen a few servers that saturate these slots pretty badly.


// Used to register a new id on creation of an object that we want to sync.
int32_t server_register_network_index(MemRef object); // Returns -1 if there is no space to add one.

// Used for registering new ids when getting object creation messages.
void    register_network_index_from_remote(int32_t network_id, MemRef object);

// Used when an object should not be synced anymore.
void    unregister_network_index(MemRef object);

// Used for translating remote ids in messages.
MemRef  get_object_from_network_index(int32_t network_id);

// Used to find the network id for an object, when creating packets to send over.
int32_t get_network_id_from_object(MemRef object); // returns -1 if not found.

// Initialize all the pointers.
void init_network_id();

struct SyncedObjectHeader{
    // The members that are called things like int# and pointer# are unknown.
    uint32_t max_count;
    uint32_t int1;                  //0x4
    uint32_t int2;                  //0x8
    uint32_t initialized; //bool    //0xC

    uint32_t int4;                  //0x10
    uint32_t pointer5;              //0x14 // Can't make sense out of this
    uint32_t count;                 //0x1B
    uint32_t pointer7;              //0x1C

    uint32_t pointer8;              //0x20
    uint32_t last_used_slot;        //0x24
    MemRef* translation_index;      //0x28 // same as network_translation_table
};

// Get synced objects table.
SyncedObjectHeader* synced_objects();
