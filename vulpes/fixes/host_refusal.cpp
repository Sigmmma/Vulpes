/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <hooker/hooker.hpp>

#include <vulpes/memory/signatures.hpp>

#include "host_refusal.hpp"

static Patch(host_refusal_fix1, NULL, 5, NOP_PATCH, 0);
static Patch(host_refusal_fix2, NULL, {0xEB});
static Patch(client_refusal_fix, NULL, {0xEB, 0x13});

void init_host_refusal_fixes() {
    if (host_refusal_fix1.build(sig_fix_host_refusal1())
    &&  host_refusal_fix2.build(sig_fix_host_refusal2())) {
        host_refusal_fix1.apply();
        host_refusal_fix2.apply();
    }
    if (client_refusal_fix.build(sig_fix_client_refusal()))
        client_refusal_fix.apply();
}

void revert_host_refusal_fixes() {
    host_refusal_fix1.revert();
    host_refusal_fix2.revert();
    client_refusal_fix.revert();
}
