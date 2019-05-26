/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include "string_overflows.hpp"
#include "../../hooker/hooker.hpp"
#include "../functions/messaging.hpp"
#include "../functions/devmode.hpp"

// HSC has a print function which does not accept extra arguments like a format print would.
// Problem is that the string gets funnelled into a console function that does use a format string.
// Thus adding any format string markers such as %s and %d will end up printing garbage, or worse, exception.

__attribute__((fastcall))
void hs_print_cleanser(char* string){
    if (developer_mode_level() >= 4) cprintf_info("%s", string);
}

Signature(false, sig_hs_print_overflow_fix,
    {0x83, 0xC4, 0x10, 0x85, 0xC0, 0x74, -1, 0x8B, 0x08, 0xA1, -1, -1, -1, -1, 0x51, 0xE8});
Patch(hs_print_overflow_fix, sig_hs_print_overflow_fix, 15, 5, CALL_PATCH, &hs_print_cleanser);

void init_string_overflow_fixes(){
    if (hs_print_overflow_fix.build()) hs_print_overflow_fix.apply();
}

void revert_string_overflow_fixes(){
    hs_print_overflow_fix.revert();
}
