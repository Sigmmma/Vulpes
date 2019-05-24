/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include "network_globals.hpp"
#include "../../hooker/hooker.hpp"

Signature(true, sig_connection_type,
    {0x66, 0x83, 0x3D, -1, -1, -1, -1, 0x02, 0x75, -1, 0x8B});

ConnectionType get_connection_type(){
    static ConnectionType* connection_type_ptr =
        *reinterpret_cast<ConnectionType**>(sig_connection_type.address() + 3);
    return *connection_type_ptr;
}
