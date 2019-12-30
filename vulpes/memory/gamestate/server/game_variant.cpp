/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <vulpes/memory/signatures.hpp>

#include "game_variant.hpp"

GameVariantData* game_variant_settings() {
    return *reinterpret_cast<GameVariantData**>(sig_game_variant_settings());
}
