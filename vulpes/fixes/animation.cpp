/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <hooker/hooker.hpp>

#include "animation.hpp"

Signature(false, sig_player_jump_turn_fix,
    {0xF6, 0x85, 0xF4, 0x02, 0x00, 0x00, 0x01, 0x0F, 0x85});

extern "C" {
    intptr_t animation__jmp_no_turn_anim;
    intptr_t animation__jmp_original_code;

    extern player_biped_mid_air_turn_fix_code();
}

Patch(player_biped_mid_air_turn_fix, sig_player_jump_turn_fix, 0, 13, JMP_PATCH, &player_biped_mid_air_turn_fix_code);

void init_animation_bug_fixes(){
    // Player Biped Mid-Air turn fix.
    static intptr_t sig_addr = sig_player_jump_turn_fix.address();
    if (sig_addr && !player_biped_mid_air_turn_fix.is_built()){
        player_biped_mid_air_turn_fix.build();
        animation__jmp_no_turn_anim = get_call_address(sig_addr+7);
        animation__jmp_original_code = player_biped_mid_air_turn_fix.return_address();
    }
    if (player_biped_mid_air_turn_fix.is_built()){
        player_biped_mid_air_turn_fix.apply();
    }
}

void revert_animation_bug_fixes(){
    player_biped_mid_air_turn_fix.revert();
}
