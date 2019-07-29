#
# Vulpes (c) 2019 gbMichelle
#
# This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
#

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
        jmp [_jmp_no_turn_anim]
    back_to_original_code:
        jmp [_jmp_original_code]

.att_syntax
