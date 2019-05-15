#pragma once
#include "../event.hpp"

DEFINE_EVENT_HOOK(EVENT_PRE_MAP_LOAD, void, char*);
DEFINE_EVENT_HOOK(EVENT_MAP_LOAD, void, char*);

DEFINE_EVENT_HOOK(EVENT_PRE_MAP_LOAD_MP, void, char*);
DEFINE_EVENT_HOOK(EVENT_MAP_LOAD_MP, void, char*);

DEFINE_EVENT_HOOK(EVENT_PRE_MAP_LOAD_SP_UI, void, char*);
DEFINE_EVENT_HOOK(EVENT_MAP_LOAD_SP_UI, void, char*);

void init_map_hooks(bool is_server);
void revert_map_hooks();
