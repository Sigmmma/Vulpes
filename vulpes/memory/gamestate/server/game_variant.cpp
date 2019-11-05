/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <hooker/hooker.hpp>

#include "game_variant.hpp"

Signature(true, sig_game_variant_settings,
    {0x80, 0x3D, -1, -1, -1, -1, 0x00, 0x74, 0x49, 0x0F,
     0xBF, 0xC9, 0x83, 0xF9, 0x01, 0x74, 0x2B});

GameVariantData* game_variant_settings(){
    static GameVariantData* game_variant_ptr =
        *reinterpret_cast<GameVariantData**>(sig_game_variant_settings.address()+2);
    return game_variant_ptr;
}
