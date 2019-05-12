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
static Signature(false, sig_sp_map_name,
    {-1,-1,-1,-1, 0xC6, 0x05,-1,-1,-1,-1, 0x00, 0xE8,-1,-1,-1,-1,
     0x8A, 0x44, 0x24, 0x1F});

char dummy_string[256] = "";
char* mp_map_name = 0;
char* sp_map_name = 0;

bool before_sp(){
    if (sp_map_name){
        call_in_order(pre_map, sp_map_name);
        call_in_order(pre_sp, sp_map_name);
    }else{
        call_in_order(pre_map, &dummy_string[0]);
        call_in_order(pre_sp, &dummy_string[0]);
    };
    return true;
}

void after_sp(){
    if (sp_map_name){
        call_in_order(post_map, sp_map_name);
        call_in_order(post_sp, sp_map_name);
    }else{
        call_in_order(post_map, &dummy_string[0]);
        call_in_order(post_sp, &dummy_string[0]);
    };
}

bool before_mp(){
    if (mp_map_name){
        call_in_order(pre_map, mp_map_name);
        call_in_order(pre_mp, mp_map_name);
    }else{
        call_in_order(pre_map, &dummy_string[0]);
        call_in_order(pre_mp, &dummy_string[0]);
    };
    return false;
}

void after_mp(){
    if (mp_map_name){
        call_in_order(post_map, mp_map_name);
        call_in_order(post_mp, mp_map_name);
    }else{
        call_in_order(post_map, &dummy_string[0]);
        call_in_order(post_mp, &dummy_string[0]);
    };
}

Cave(map_load_ui_sp_hook, (void*)&before_sp, (void*)&after_sp);
Cave(map_load_mp_hook, (void*)&before_mp, (void*)&after_mp);

void init_map_hooks(){
    map_load_ui_sp_hook.build(sig_map_load_ui_sp.get_address(), 5);
    map_load_mp_hook.build(sig_map_load_mp.get_address(), 6);
    mp_map_name = (char*)*(uintptr_t*)sig_map_name.get_address();
    sp_map_name = (char*)*(uintptr_t*)sig_sp_map_name.get_address();
    map_load_ui_sp_hook.apply();
    map_load_mp_hook.apply();
}

void revert_map_hooks(){
    map_load_ui_sp_hook.revert();
    map_load_mp_hook.revert();
}
