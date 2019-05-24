/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#define "../../../hooker/hooker.hpp"

Signature(true, sig_bool_allow_client_side_projectiles,
    {-1, -1, -1, -1, 0x01, 0x74, 0x15, 0x33, 0xC0, 0xEB, 0x16});

static bool* allow_client_side_projectiles;

void init_foxnet_projectile(){
    allow_client_side_projectiles =
        (bool*)*(uintptr_t*)sig_bool_allow_client_side_projectiles.address();

}
