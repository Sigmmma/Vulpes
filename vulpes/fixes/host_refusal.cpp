/*
 * This file is part of Vulpes, an extension of Halo Custom Edition's capabilities.
 * Copyright (C) 2019-2020 gbMichelle (Michelle van der Graaf)
 *
 * Vulpes is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, version 3.
 *
 * Vulpes is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * long with Vulpes.  If not, see <https://www.gnu.org/licenses/agpl-3.0.en.html>
 */

#include <hooker/hooker.hpp>

#include <vulpes/memory/signatures.hpp>

#include "host_refusal.hpp"

static Patch(host_refusal_fix1, NULL, 5, NOP_PATCH, 0);
static Patch(host_refusal_fix2, NULL, {JMP_SMALL_BYTE});
static Patch(client_refusal_fix, NULL, {JMP_SMALL_BYTE, 0x13}); // Relative jump 0x13 bytes forward.

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
