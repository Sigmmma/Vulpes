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
