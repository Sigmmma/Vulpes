/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include "host_refusal.hpp"
#include "../../hooker/hooker.hpp"

Signature(false, sig_host_refusal1,
    {0x88, 0x5C, 0x24, 0x2A, 0xE8, -1, -1, -1, -1, 0x50, 0xE8});
Signature(false, sig_host_refusal2,
    {0x8B, 0x7F, 0x08, 0x3B, 0xFD, 0x74, 0x5F, 0x8D, 0x9B, 0x00, 0x00, 0x00, 0x00});
Signature(false, sig_client_refusal,
    {-1, -1, 0x75, 0x11, 0x6A, 0x01, 0x6A, 0x7E, 0x68, 0xA0, 0x00, 0x00, 0x00});

Patch(host_refusal_fix1, sig_host_refusal1, 0xA, 5, NOP_PATCH, 0);
Patch(host_refusal_fix2, sig_host_refusal2, 5, {0xEB});
Patch(client_refusal_fix, sig_client_refusal, 0, {0xEB, 0x13});

void init_host_refusal_fixes(){
    if (host_refusal_fix1.build() && host_refusal_fix2.build()){
        host_refusal_fix1.apply();
        host_refusal_fix2.apply();
    }
    if (client_refusal_fix.build()) client_refusal_fix.apply();
}

void revert_host_refusal_fixes(){
    host_refusal_fix1.revert();
    host_refusal_fix2.revert();
    client_refusal_fix.revert();
}
