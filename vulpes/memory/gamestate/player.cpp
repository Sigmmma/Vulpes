/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <vulpes/memory/signatures.hpp>

#include "player.hpp"

PlayerTable* player_table() {
    return **reinterpret_cast<PlayerTable***>(sig_player_table_ref());
}
