/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <hooker/hooker.hpp>

#include <vulpes/memory/signatures.hpp>

#include "map.hpp"

DEFINE_EVENT_HOOK_LIST(EVENT_PRE_MAP_LOAD, pre_map);
DEFINE_EVENT_HOOK_LIST(EVENT_MAP_LOAD, post_map);

DEFINE_EVENT_HOOK_LIST(EVENT_PRE_MAP_LOAD_MP, pre_mp);
DEFINE_EVENT_HOOK_LIST(EVENT_MAP_LOAD_MP, post_mp);

DEFINE_EVENT_HOOK_LIST(EVENT_PRE_MAP_LOAD_SP_UI, pre_sp);
DEFINE_EVENT_HOOK_LIST(EVENT_MAP_LOAD_SP_UI, post_sp);

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
    map_load_ui_sp_hook_patch, 0, 5,
    JMP_PATCH, &map_load_sp_wrapper
);
Patch(
    map_load_mp_hook_patch, 0, 6,
    JMP_PATCH, &map_load_mp_wrapper
);

void init_map_hooks(bool is_server) {
    if (!is_server) {
        map_load_ui_sp_hook_patch.build(sig_hook_map_load_ui_sp());
        map_load_ui_sp_hook_patch.apply();
        map_load_sp_actual_jmp = map_load_ui_sp_hook_patch.return_address();
    }
    map_load_mp_hook_patch.build(sig_hook_map_load_mp());
    map_load_mp_hook_patch.apply();
    map_load_mp_actual_jmp = map_load_mp_hook_patch.return_address();
}

void revert_map_hooks() {
    map_load_ui_sp_hook_patch.revert();
    map_load_mp_hook_patch.revert();
}
