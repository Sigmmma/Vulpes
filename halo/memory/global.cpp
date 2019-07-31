/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include "global.hpp"
#include "../../hooker/hooker.hpp"

Signature(true, sig_path,
    {0x68, -1, -1, -1, -1, 0xE8, -1, -1, -1, -1, 0x83, 0xC4, 0x04, 0x88,
     0x1D, -1, -1, -1, -1, 0xC6, 0x05, -1, -1, -1, -1, 0x01, 0xC6, 0x05});

char* profile_path(){
    static char* path_pointer =
        *reinterpret_cast<char**>(sig_path.address()+1);
    return path_pointer;
}
