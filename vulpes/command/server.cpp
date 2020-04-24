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

#include <vulpes/functions/message_delta.hpp>
#include <vulpes/functions/messaging.hpp>
#include <vulpes/memory/network/network.hpp>
#include <vulpes/memory/server/king.hpp>

#include "handler.hpp"
#include "server.hpp"

bool cmd_rprint_func(std::vector<VulpesArg> input) {
    if (game_is_server_executable()) {
        int player_id = input[0].int_out();
        if (player_id < 16) {
            rprintf(player_id, input[1].str_out().data());
        } else {
            cprintf_error("Player id: %d is too high.", player_id);
        }
    } else {
        cprintf_error("v_sv_print is a Vulpes dedicated-server-only command.");
    }
    return true;
}

bool cmd_sv_say_func(std::vector<VulpesArg> input) {
    int player_id = input[0].int_out();
    if (player_id < 16) {
        chatf(HudChatType::SERVER, -1, player_id,
              "%s", input[1].str_out().data());
    } else {
        cprintf_error("Player id: %d is too high.", player_id);
    }
    return true;
}

bool cmd_sv_hill_move_func(std::vector<VulpesArg> input) {
    king_globals()->current_hill_time_left = 1;
    cprintf("Hill moved.");
    return true;
}

bool cmd_sv_hill_timer_func(std::vector<VulpesArg> input) {
    auto king_globs = king_globals_upgrade();
    int32_t ticks = input[0].int_out();
    king_globs->hill_length = ticks;
    if (input.size() == 2 && input[1].bool_out()) {
        king_globals()->current_hill_time_left = ticks;
        cprintf("Hill timer is reset to, and will be %d seconds.", ticks/30);
    } else {
        cprintf("Hill timer will be %d seconds from the next time the hill moves.", ticks/30);
    }
    return true;
}

void init_server_commands() {
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
