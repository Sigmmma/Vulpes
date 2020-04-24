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

#include <hooker/hooker.hpp>

#include <vulpes/memory/signatures/signatures.hpp>

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

static Patch(
    map_load_ui_sp_hook_patch, NULL, 5,
    JMP_PATCH, &map_load_sp_wrapper
);
static Patch(
    map_load_mp_hook_patch, NULL, 6,
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
