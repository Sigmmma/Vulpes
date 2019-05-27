/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include "server.hpp"
#include "handler.hpp"
#include "../halo/functions/messaging.hpp"

bool cmd_rprint_func(std::vector<VulpesArg> input){
    int player_id = input[0].int_out();
    if (player_id < 0){
        rprintf_all("%s", input[1].str_out().data());
    }else if (player_id < 16){
        rprintf(player_id, "%s", input[1].str_out().data());
    }else{
        cprintf_error("Player id: %d is too high.", player_id);
    };
    return true;
}

void init_server_commands(){
    static VulpesCommand cmd_rprint(
        "v_sv_rprint", &cmd_rprint_func, 0, 2,
        VulpesArgDef("player", true, A_CHAR),
        VulpesArgDef("text", true, A_STRING)
    );
}
