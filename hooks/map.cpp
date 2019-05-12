#include "map.hpp"
#include "hooker.hpp"

DEFINE_EVENT_HOOK_LIST(EVENT_PRE_MAP_LOAD, pre_map);
DEFINE_EVENT_HOOK_LIST(EVENT_MAP_LOAD, post_map);

DEFINE_EVENT_HOOK_LIST(EVENT_PRE_MAP_LOAD_MP, pre_mp);
DEFINE_EVENT_HOOK_LIST(EVENT_MAP_LOAD_MP, post_mp);

DEFINE_EVENT_HOOK_LIST(EVENT_PRE_MAP_LOAD_SP_UI, pre_sp);
DEFINE_EVENT_HOOK_LIST(EVENT_MAP_LOAD_SP_UI, post_sp);

static Signature(true, sig_map_load_ui_sp,
    {0x83, 0xEC, 0x0C, 0x53, 0x55, 0x56, 0x57, 0x8B, 0xF0, 0xE8});

static Signature(true, sig_map_load_mp,
    {0x81, 0xEC, 0x10, 0x01, 0x00, 0x00, 0x53, 0x55, 0x8B, 0xAC,
     0x24, 0x1C, 0x01, 0x00, 0x00});

static Signature(true, sig_map_name,
    {-1,-1,-1,-1, 0xE8,-1,-1,-1,-1, 0x32, 0xC9, 0x83, 0xF8, 0x13, 0x7D});

char* map_name;

bool before_sp(){
    call_in_order(pre_map, map_name);
    call_in_order(pre_sp, map_name);
    return true;
}

void after_sp(){
    call_in_order(post_map, map_name);
    call_in_order(post_sp, map_name);
}

bool before_mp(){
    call_in_order(pre_map, map_name);
    call_in_order(pre_mp, map_name);
    return true;
}

void after_mp(){
    call_in_order(post_map, map_name);
    call_in_order(post_mp, map_name);
}

Cave(map_load_ui_sp_hook, (void*)&before_sp, (void*)&after_sp);
Cave(map_load_mp_hook, (void*)&before_mp, (void*)&after_mp);

void init_map_hooks(){
    map_load_ui_sp_hook.build(sig_map_load_ui_sp.get_address(), 5);
    map_load_mp_hook.build(sig_map_load_mp.get_address(), 6);
    map_name = (char*)*(uintptr_t*)sig_map_name.get_address();
    map_load_ui_sp_hook.apply();
    map_load_mp_hook.apply();
}

void revert_map_hooks(){
    map_load_ui_sp_hook.revert();
    map_load_mp_hook.revert();
}
