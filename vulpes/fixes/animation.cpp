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

#include "animation.hpp"

extern "C" {
    intptr_t animation__jmp_no_turn_anim;
    intptr_t animation__jmp_original_code;

    extern player_biped_mid_air_turn_fix_code();
}

static Patch(player_biped_mid_air_turn_fix, NULL, 13,
    JMP_PATCH, &player_biped_mid_air_turn_fix_code);

void init_animation_bug_fixes() {
    // Player Biped Mid-Air turn fix.
    auto sig_addr = sig_fix_player_jump_turn();
    if (sig_addr && !player_biped_mid_air_turn_fix.is_built()) {
        player_biped_mid_air_turn_fix.build(sig_addr);
        animation__jmp_no_turn_anim = get_call_address(sig_addr+7);
        animation__jmp_original_code = player_biped_mid_air_turn_fix.return_address();
    }
    if (player_biped_mid_air_turn_fix.is_built()) {
        player_biped_mid_air_turn_fix.apply();
    }
}

void revert_animation_bug_fixes() {
    player_biped_mid_air_turn_fix.revert();
}
