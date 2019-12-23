/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <vulpes/memory/signatures.hpp>

#include "network.hpp"

ConnectionType* connection_type() {
    return *reinterpret_cast<ConnectionType**>(sig_connection_type_enum());
}

bool* allow_client_side_projectiles() {
    return *reinterpret_cast<bool**>(sig_allow_client_side_projectiles_bool());
}

bool game_is_server_executable() {
    return sig_server() != 0;
}
