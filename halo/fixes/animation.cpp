#include "animation.hpp"
#include "../../hooker/hooker.hpp"

Signature(false, sig_player_jump_turn_fix,
    {0xF6, 0x85, 0xF4, 0x02, 0x00, 0x00, 0x01, 0x0F, 0x85});

static intptr_t jmp_no_turn_anim;
static intptr_t jmp_original_code;

__attribute__((naked))
void player_biped_mid_air_turn_fix_code(){
    asm (
        // test is weird, read jne as jump if equals, and je as jump if not equal.
        "test byte ptr [ebp+0x2F4], 1;" // tag: turns_without_animating
        "jne no_turn_anim;"
        "test byte ptr [ebp+0x2F4], 2;" // tag: uses_player_physics
        "je back_to_original_code;"
        "test byte ptr [edi+0x4CC], 1;" // obj: is_airborne
        "je back_to_original_code;"
    "no_turn_anim:"
        "jmp %0;"
    "back_to_original_code:"
        "jmp %1;"
        :
        : "m" (jmp_no_turn_anim), "m" (jmp_original_code)
    );
}

PatchNew(player_buped_mid_air_turn_fix, sig_player_jump_turn_fix, 0, 13, JMP_PATCH, &player_biped_mid_air_turn_fix_code);

void init_animation_bug_fixes(){
    // Player Biped Mid-Air turn fix.
    static intptr_t sig_addr = sig_player_jump_turn_fix.address();
    if (sig_addr && !player_buped_mid_air_turn_fix.is_built()){
        player_buped_mid_air_turn_fix.build();
        jmp_no_turn_anim = get_call_address(sig_addr+7);
        jmp_original_code = player_buped_mid_air_turn_fix.get_return_address();
    };
    if (player_buped_mid_air_turn_fix.is_built()){
        player_buped_mid_air_turn_fix.apply();
    };
}

void revert_animation_bug_fixes(){
    player_buped_mid_air_turn_fix.revert();
}

void init_animation_bug_fixes_e(char* dummy){init_animation_bug_fixes();}
void revert_animation_bug_fixes_e(char* dummy){revert_animation_bug_fixes();}
