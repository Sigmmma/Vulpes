/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#pragma once

////// These are functions for using the existing object syncing infrastructure.
////// I really have to stress that we shouldn't use these too much if we start
////// syncronizing things that normally have no network ids.
////// There is only 511 network id slots, and when figuring this all out
////// I have seen a few servers that saturate these slots pretty badly.

////// Everything in here is currently untested. This may very well not work.


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
