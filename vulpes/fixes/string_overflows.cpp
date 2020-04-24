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

#include <vulpes/memory/signatures/signatures.hpp>
#include <vulpes/functions/messaging.hpp>

#include "string_overflows.hpp"

// HSC has a print function which does not accept extra arguments like a format print would.
// Problem is that the string gets funnelled into a console function that does use a format string.
// Thus adding any format string markers such as %s and %d will end up printing garbage, or worse, exception.

__attribute__((fastcall))
void hs_print_cleanser(char* string) {
    if (sig_developer_mode_level() && *sig_developer_mode_level() >= 4)
        cprintf_info("%s", string);
}

static Patch(hs_print_overflow_fix, NULL, 5, CALL_PATCH, &hs_print_cleanser);

void init_string_overflow_fixes() {
    if (hs_print_overflow_fix.build(sig_fix_hs_print_overflow()))
        hs_print_overflow_fix.apply();
}

void revert_string_overflow_fixes() {
    hs_print_overflow_fix.revert();
}
