/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include "debug.hpp"
#include "handler.hpp"
#include "../functions/messaging.hpp"
#include "../fixes/shdr_trans_zfighting.hpp"
#include "../memory/gamestate/network.hpp"
#include "../network/network_id.hpp"

bool toggle_shader_trans_fix(std::vector<VulpesArg> input){
    bool on = input[0].bool_out();
    if (on){
        init_shdr_trans_zfighting_fixes();
        cprintf("Turned ON transparent fix.");
    }else{
        revert_shdr_trans_zfighting_fixes();
        cprintf("Turned OFF transparent fix.");
    };
    return true;
}

bool toggle_allow_client_side_projectiles(std::vector<VulpesArg> input){
    if (input.size() > 0){
        bool on = input[0].bool_out();
        *allow_client_side_projectiles() = on;
        if (on){
            cprintf("Turned ON client side projectiles.");
        }else{
            cprintf("Turned OFF client side projectiles.");
        };
    }else{
        if (*allow_client_side_projectiles()){
            cprintf("Client side projectiles are ON.");
        }else{
            cprintf("Client side projectiles are OFF.");
        };
    };
    return true;
}

bool get_network_id_from_obj_id(std::vector<VulpesArg> input){
    uint32_t id = input[0].int_out();
    cprintf("%d", get_network_id_from_object(*reinterpret_cast<MemRef*>(&id)));
    return true;
}

bool get_object_id_from_network_id(std::vector<VulpesArg> input){
    uint32_t id = input[0].int_out();
    cprintf("0x%X", get_object_from_network_index(id));
    return true;
}

void init_debug_commands(){
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
}
