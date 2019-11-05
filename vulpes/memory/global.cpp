/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <hooker/hooker.hpp>

#include <vulpes/memory/gamestate/network.hpp>

#include "global.hpp"

Signature(true, sig_path,
    {0x68, -1, -1, -1, -1, 0xE8, -1, -1, -1, -1, 0x83, 0xC4, 0x04, 0x88,
     0x1D, -1, -1, -1, -1, 0xC6, 0x05, -1, -1, -1, -1, 0x01, 0xC6, 0x05});

char* profile_path() {
    static char* path_pointer =
        *reinterpret_cast<char**>(sig_path.address()+1);
    return path_pointer;
}

Signature(true, sig_map_name,
    {-1,-1,-1,-1, 0xE8,-1,-1,-1,-1, 0x32, 0xC9, 0x83, 0xF8, 0x13, 0x7D});
Signature(true, sig_sp_map_name,
    {-1,-1,-1,-1, 0xC6, 0x05,-1,-1,-1,-1, 0x00, 0xE8,-1,-1,-1,-1,
     0x8A, 0x44, 0x24, 0x1F});

char* map_name() {
    char* name_pointer = NULL;
    if (*connection_type() == ConnectionType::NONE) {
        name_pointer = *reinterpret_cast<char**>(sig_sp_map_name.address());
    } else {
        name_pointer = *reinterpret_cast<char**>(sig_map_name.address());
    }
    return name_pointer;
}
