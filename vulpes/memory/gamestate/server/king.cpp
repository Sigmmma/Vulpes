/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <hooker/hooker.hpp>

#include "king.hpp"

Signature(true, sig_king_globals,
    {0xB9, 0x6B, 0x00, 0x00, 0x00, 0xBF, -1, -1, -1, -1,
     0xF3, 0xAB, 0xB9, 0x6B, 0x00, 0x00, 0x00});

KingGlobals* king_globals() {
    static KingGlobals* king_globals_ptr =
        *reinterpret_cast<KingGlobals**>(sig_king_globals.address()+6);
    return king_globals_ptr;
}

KingGlobalsUpgrade king_globals_upgrade_data;

KingGlobalsUpgrade* king_globals_upgrade() {
    return &king_globals_upgrade_data;
}
