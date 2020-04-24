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

#include <vulpes/hooks/tick.hpp>
#include <vulpes/fixes/shdr_trans_zfighting.hpp>
#include <vulpes/functions/messaging.hpp>
#include <vulpes/memory/network/network.hpp>
#include <vulpes/network/network_id.hpp>
#include <vulpes/debug/budget.hpp>

#include "debug.hpp"
#include "handler.hpp"


bool toggle_shader_trans_fix(std::vector<VulpesArg> input) {
    bool on = input[0].bool_out();
    if (on) {
        init_shdr_trans_zfighting_fixes();
        cprintf("Turned ON transparent fix.");
    } else {
        revert_shdr_trans_zfighting_fixes();
        cprintf("Turned OFF transparent fix.");
    }
    return true;
}

bool toggle_allow_client_side_projectiles(std::vector<VulpesArg> input) {
    if (input.size() > 0) {
        bool on = input[0].bool_out();
        *allow_client_side_projectiles() = on;
        if (on) {
            cprintf("Turned ON client side projectiles.");
        } else {
            cprintf("Turned OFF client side projectiles.");
        }
    } else {
        if (*allow_client_side_projectiles()) {
            cprintf("Client side projectiles are ON.");
        } else {
            cprintf("Client side projectiles are OFF.");
        }
    }
    return true;
}

bool get_network_id_from_obj_id(std::vector<VulpesArg> input) {
    uint32_t id = input[0].int_out();
    cprintf("%d", get_network_id_from_object(*reinterpret_cast<MemRef*>(&id)));
    return true;
}

bool get_object_id_from_network_id(std::vector<VulpesArg> input) {
    uint32_t id = input[0].int_out();
    cprintf("0x%X", get_object_from_network_index(id));
    return true;
}

bool toggle_cprint_budget(std::vector<VulpesArg> input) {
    bool on = input[0].bool_out();

    if (on) {
        ADD_CALLBACK_P(EVENT_TICK, cprint_budget, EVENT_PRIORITY_FINAL);
    } else {
        DEL_CALLBACK(EVENT_TICK, cprint_budget);
    }

    return true;
}

static bool print_about(std::vector<VulpesArg> input) {
    cprintf("%s", "Vulpes is an extension of Halo Custom Edition's capabilities.");
    cprintf("%s", "Copyright (C) 2019-2020 gbMichelle");
    cprintf("%s", "");
    cprintf("%s", "Vulpes is free software: you can redistribute it and/or modify");
    cprintf("%s", "it under the terms of the GNU Affero General Public License as");
    cprintf("%s", "published by the Free Software Foundation, version 3.");
    cprintf("%s", "");
    cprintf("%s", "Vulpes is distributed in the hope that it will be useful,");
    cprintf("%s", "but WITHOUT ANY WARRANTY; without even the implied warranty of");
    cprintf("%s", "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the");
    cprintf("%s", "GNU Affero General Public License for more details.");
    cprintf("%s", "");
    cprintf("%s", "You should have received a copy of the GNU Affero General Public License");
    cprintf("%s", "along with Vulpes.  If not, see <https://www.gnu.org/licenses/agpl-3.0.en.html>.");
    cprintf("%s", "");
    cprintf("%s", "Check out the source code at https://github.com/Sigmmma/Vulpes");
    return true;
}

void init_debug_commands() {
    static VulpesCommand cmd_dev_shader_transparent_fix(
        "v_dev_shader_transparent_fix",
        &toggle_shader_trans_fix, 4, 1,
        VulpesArgDef("", true, A_BOOL)
    );
    static VulpesCommand cmd_dev_allow_client_side_projectiles(
        "v_dev_allow_client_side_projectiles",
        &toggle_allow_client_side_projectiles, 4, 1,
        VulpesArgDef("", false, A_BOOL)
    );

    static VulpesCommand cmd_get_network_id_from_obj_id(
        "v_dev_get_network_id_from_obj_id",
        &get_network_id_from_obj_id, 4, 1,
        VulpesArgDef("", true, A_LONG)
    );
    static VulpesCommand cmd_get_object_id_from_network_id(
        "v_dev_get_object_id_from_network_id",
        &get_object_id_from_network_id, 4, 1,
        VulpesArgDef("", true, A_LONG)
    );

    static VulpesCommand cmd_toggle_cprint_budget(
        "v_dev_show_budget",
        &toggle_cprint_budget, 0, 1,
        VulpesArgDef("", true, A_BOOL)
    );

    static VulpesCommand cmd_print_about(
        "v_about",
        &print_about, 0, 0
    );
}
