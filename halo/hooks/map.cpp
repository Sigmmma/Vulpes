/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include "map.hpp"
#include "../../hooker/hooker.hpp"
#include "../memory/types.hpp"

DEFINE_EVENT_HOOK_LIST(EVENT_PRE_MAP_LOAD, pre_map);
DEFINE_EVENT_HOOK_LIST(EVENT_MAP_LOAD, post_map);

DEFINE_EVENT_HOOK_LIST(EVENT_PRE_MAP_LOAD_MP, pre_mp);
DEFINE_EVENT_HOOK_LIST(EVENT_MAP_LOAD_MP, post_mp);

DEFINE_EVENT_HOOK_LIST(EVENT_PRE_MAP_LOAD_SP_UI, pre_sp);
DEFINE_EVENT_HOOK_LIST(EVENT_MAP_LOAD_SP_UI, post_sp);

Signature(true, sig_map_load_ui_sp,
    {0x83, 0xEC, 0x0C, 0x53, 0x55, 0x56, 0x57, 0x8B, 0xF0, 0xE8});

Signature(true, sig_map_load_mp,
    {0x81, 0xEC, 0x10, 0x01, 0x00, 0x00, 0x53, 0x55, 0x8B, 0xAC,
     0x24, 0x1C, 0x01, 0x00, 0x00});

Signature(true, sig_map_name,
    {-1,-1,-1,-1, 0xE8,-1,-1,-1,-1, 0x32, 0xC9, 0x83, 0xF8, 0x13, 0x7D});
Signature(false, sig_sp_map_name,
    {-1,-1,-1,-1, 0xC6, 0x05,-1,-1,-1,-1, 0x00, 0xE8,-1,-1,-1,-1,
     0x8A, 0x44, 0x24, 0x1F});

Signature(true, sig_event_map_reset_hook, {0x5B, 0x68, -1, -1, -1, -1, 0x33, 0xC0});

char* mp_map_name = NULL;
char* sp_map_name = NULL;

bool before_map_load_sp(){
    call_in_order(pre_map);
    call_in_order(pre_sp);
    return true;
}

void after_map_load_sp(){
    call_in_order(post_map);
    call_in_order(post_sp);
}

bool before_map_load_mp(){
    call_in_order(pre_map);
    call_in_order(pre_mp);
    return true;
}

void after_map_load_mp(){
    call_in_order(post_map);
    call_in_order(post_mp);
}

Cave(
    map_load_ui_sp_hook,
    sig_map_load_ui_sp, 0, 5,
    &before_map_load_sp, &after_map_load_sp
);
Cave(
    map_load_mp_hook,
    sig_map_load_mp, 0, 6,
    &before_map_load_mp, &after_map_load_mp
);

void init_map_hooks(bool is_server){
    if (!is_server){
        map_load_ui_sp_hook.build();
        sp_map_name = *reinterpret_cast<char**>(sig_sp_map_name.address());
        map_load_ui_sp_hook.apply();
    };
    map_load_mp_hook.build();
    mp_map_name = *reinterpret_cast<char**>(sig_map_name.address());
    map_load_mp_hook.apply();
}

void revert_map_hooks(){
    map_load_ui_sp_hook.revert();
    map_load_mp_hook.revert();
}
