/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include "player.hpp"

Signature(true, sig_player_table,
    {-1, -1, -1, 0x00, 0x89, 0x44, 0x24, 0x48, 0x35, 0x72, 0x65, 0x74, 0x69});

PlayerTable* player_table(){
    static PlayerTable** player_table_ptr =
        reinterpret_cast<PlayerTable**>(sig_player_table.address());
    return *player_table_ptr;
}
