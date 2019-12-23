/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <vulpes/memory/signatures.hpp>

#include "king.hpp"

KingGlobals* king_globals() {
    return *reinterpret_cast<KingGlobals**>(sig_king_globals());
}

KingGlobalsUpgrade king_globals_upgrade_data;

KingGlobalsUpgrade* king_globals_upgrade() {
    return &king_globals_upgrade_data;
}
