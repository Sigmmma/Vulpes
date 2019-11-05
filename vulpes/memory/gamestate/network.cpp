/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <hooker/hooker.hpp>

#include "network.hpp"

Signature(false, sig_server,
    { 0x75, 0x2D, 0x68, -1, -1, -1, -1, 0xE8, -1, -1, -1, -1,
      0x68, -1, -1, -1, -1, 0x68, -1, -1, -1, -1, 0xE8, -1, -1, -1, -1,
      0x68, -1, -1, -1, -1, 0x68, -1, -1, -1, -1, 0x33, 0xC0,
      0xE8, -1, -1, -1, -1, 0x83, 0xC4, 0x14, 0x8B, 0x44, 0x24, 0x04,
      0x68, -1, -1, -1, -1, 0x68, -1, -1, -1, -1, 0x50 });
Signature(true, sig_connection_type,
    {0x66, 0x83, 0x3D, -1, -1, -1, -1, 0x02, 0x75, -1, 0x8B});
Signature(false, sig_allow_client_side_projectiles,
    {0x80, 0x3D, -1, -1, -1, -1, 0x01, 0x74, 0x15, 0x33, 0xC0, 0xEB});

ConnectionType* connection_type() {
    static ConnectionType* connection_type_ptr =
        *reinterpret_cast<ConnectionType**>(sig_connection_type.address() + 3);
    return connection_type_ptr;
}

bool* allow_client_side_projectiles() {
    static bool* allow_client_side_projectiles_ptr =
        *reinterpret_cast<bool**>(sig_allow_client_side_projectiles.address() + 2);
    return allow_client_side_projectiles_ptr;
}

bool game_is_server_executable() {
    static bool is_server = sig_server.address() != 0;
    return is_server;
}
