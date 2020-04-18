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

.intel_syntax noprefix

.globl _player_biped_mid_air_turn_fix_code
_player_biped_mid_air_turn_fix_code:
        test byte ptr [ebp+0x2F4], 1 # tag: turns_without_animating
        jne no_turn_anim
        test byte ptr [ebp+0x2F4], 2 # tag: uses_player_physics
        je back_to_original_code
        test byte ptr [edi+0x4CC], 1 # obj: is_airborne
        je back_to_original_code
    no_turn_anim:
        jmp [_animation__jmp_no_turn_anim]
    back_to_original_code:
        jmp [_animation__jmp_original_code]

.att_syntax
