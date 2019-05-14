#include "string_overflows.hpp"
#include "../hooks/hooker.hpp"
#include <cstdint>
#include "../halo_functions/console.hpp"
#include "../halo_functions/devmode.hpp"

// HSC has a print function which does not accept extra arguments like a format print would.
// Problem is that the string gets funnelled into a console function that does use a format string.
// Thus adding any format string markers such as %s and %d will end up printing garbage, or worse, exception.

Signature(false, sig_hs_print_overflow_fix,
    {0x83, 0xC4, 0x10, 0x85, 0xC0, 0x74, -1, 0x8B, 0x08, 0xA1, -1, -1, -1, -1, 0x51, 0xE8});
Patch(hs_print_overflow_fix);

__attribute__((fastcall))
void hs_print_cleanser(char* string){
    if (developer_mode_level() >= 4){
        ARGBFloat color;
        color.alpha = 1;
        color.red = 0;
        color.green = 1;
        color.blue = 0;
        console_out(string, color);
    };
}

void init_string_overflow_fixes(){
    if (!hs_print_overflow_fix.is_built()){
        hs_print_overflow_fix.build(sig_hs_print_overflow_fix.get_address()+15, 5,
            CALL_PATCH, (uintptr_t)&hs_print_cleanser);
    };
    hs_print_overflow_fix.apply();
}

void revert_string_overflow_fixes(){
    hs_print_overflow_fix.revert();
}
