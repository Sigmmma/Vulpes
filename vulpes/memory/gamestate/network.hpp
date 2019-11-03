/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#pragma once
#include <cstdint>

enum class ConnectionType : uint32_t {
    NONE = 0,
    CLIENT = 1,
    HOST = 2
};

ConnectionType* connection_type();
bool* allow_client_side_projectiles();
bool game_is_server_executable();
