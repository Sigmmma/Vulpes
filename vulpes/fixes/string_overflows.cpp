/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <hooker/hooker.hpp>

#include <vulpes/memory/signatures.hpp>
#include <vulpes/functions/messaging.hpp>

#include "string_overflows.hpp"

// HSC has a print function which does not accept extra arguments like a format print would.
// Problem is that the string gets funnelled into a console function that does use a format string.
// Thus adding any format string markers such as %s and %d will end up printing garbage, or worse, exception.

__attribute__((fastcall))
void hs_print_cleanser(char* string) {
    if (developer_mode_level() && *developer_mode_level() >= 4)
        cprintf_info("%s", string);
}

Patch(hs_print_overflow_fix, 0, 5, CALL_PATCH, &hs_print_cleanser);

void init_string_overflow_fixes() {
    if (hs_print_overflow_fix.build(fix_hs_print_overflow()))
        hs_print_overflow_fix.apply();
}

void revert_string_overflow_fixes() {
    hs_print_overflow_fix.revert();
}
