/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include "server.hpp"
#include "handler.hpp"
#include "../functions/messaging.hpp"
#include "../functions/message_delta.hpp"
#include "../memory/gamestate/network.hpp"
#include "../memory/gamestate/server/king.hpp"

bool cmd_rprint_func(std::vector<VulpesArg> input){
    if (game_is_server_executable()){
        int player_id = input[0].int_out();
        if (player_id < 16){
            rprintf(player_id, input[1].str_out().data());
        }else{
            cprintf_error("Player id: %d is too high.", player_id);
        }
    }else{
        cprintf_error("v_sv_print is a Vulpes dedicated-server-only command.");
    }
    return true;
}

bool cmd_sv_say_func(std::vector<VulpesArg> input){
    int player_id = input[0].int_out();
    if (player_id < 16){
        chatf(HudChatType::SERVER, -1, player_id,
              "%s", input[1].str_out().data());
    }else{
        cprintf_error("Player id: %d is too high.", player_id);
    }
    return true;
}

bool cmd_sv_hill_move_func(std::vector<VulpesArg> input){
    king_globals()->current_hill_time_left = 1;
    cprintf("Hill moved.");
    return true;
}

bool cmd_sv_hill_timer_func(std::vector<VulpesArg> input){
    auto king_globs = king_globals_upgrade();
    int32_t ticks = input[0].int_out();
    king_globs->hill_length = ticks;
    if (input.size() == 2 && input[1].bool_out()){
        king_globals()->current_hill_time_left = ticks;
        cprintf("Hill timer is reset to, and will be %d seconds.", ticks/30);
    }else{
        cprintf("Hill timer will be %d seconds from the next time the hill moves.", ticks/30);
    }
    return true;
}

void init_server_commands(){
    static VulpesCommand cmd_rprint(
        "v_sv_rprint", &cmd_rprint_func, 0, 2,
        VulpesArgDef("player", true, A_CHAR),
        VulpesArgDef("text", true, A_STRING)
    );
    static VulpesCommand cmd_sv_say(
        "v_sv_say", &cmd_sv_say_func, 0, 2,
        VulpesArgDef("player", true, A_CHAR),
        VulpesArgDef("text", true, A_STRING)
    );
    static VulpesCommand cmd_sv_hill_move(
        "v_sv_hill_move", &cmd_sv_hill_move_func, 0, 0
    );
    static VulpesCommand cmd_sv_hill_timer(
        "v_sv_hill_timer", &cmd_sv_hill_timer_func, 0, 2,
        VulpesArgDef("time", true, A_TIME),
        VulpesArgDef("reset_timer", false, A_BOOL)
    );
}
