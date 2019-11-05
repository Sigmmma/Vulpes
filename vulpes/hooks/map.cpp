/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <hooker/hooker.hpp>

#include <vulpes/memory/types.hpp>

#include "map.hpp"

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

Signature(true, sig_event_map_reset_hook, {0x5B, 0x68, -1, -1, -1, -1, 0x33, 0xC0});

extern "C" bool before_map_load_sp() {
    call_in_order(pre_map);
    call_in_order(pre_sp);
    return true;
}

extern "C" void after_map_load_sp() {
    call_in_order(post_map);
    call_in_order(post_sp);
}

extern "C" bool before_map_load_mp() {
    call_in_order(pre_map);
    call_in_order(pre_mp);
    return true;
}

extern "C" void after_map_load_mp() {
    call_in_order(post_map);
    call_in_order(post_mp);
}

extern "C" {

    uintptr_t map_load_sp_actual_jmp;
    extern map_load_sp_wrapper();

    uintptr_t map_load_mp_actual_jmp;
    extern map_load_mp_wrapper();

}

Patch(
    map_load_ui_sp_hook_patch,
    sig_map_load_ui_sp, 0, 5,
    JMP_PATCH, &map_load_sp_wrapper
);
Patch(
    map_load_mp_hook_patch,
    sig_map_load_mp, 0, 6,
    JMP_PATCH, &map_load_mp_wrapper
);

void init_map_hooks(bool is_server) {
    if (!is_server) {
        map_load_ui_sp_hook_patch.build();
        map_load_ui_sp_hook_patch.apply();
        map_load_sp_actual_jmp = map_load_ui_sp_hook_patch.return_address();
    }
    map_load_mp_hook_patch.build();
    map_load_mp_hook_patch.apply();
    map_load_mp_actual_jmp = map_load_mp_hook_patch.return_address();
}

void revert_map_hooks() {
    map_load_ui_sp_hook_patch.revert();
    map_load_mp_hook_patch.revert();
}
