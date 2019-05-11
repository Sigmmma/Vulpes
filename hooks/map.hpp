#pragma once
#include "event.hpp"

DEFINE_EVENT_HOOK(EVENT_PRE_MAP_LOAD, void, char* map_name);
DEFINE_EVENT_HOOK(EVENT_MAP_LOAD, void, char* map_name);

DEFINE_EVENT_HOOK(EVENT_PRE_MAP_LOAD_MP, void, char* map_name);
DEFINE_EVENT_HOOK(EVENT_MAP_LOAD_MP, void, char* map_name);

DEFINE_EVENT_HOOK(EVENT_PRE_MAP_LOAD_SP_UI, void, char* map_name);
DEFINE_EVENT_HOOK(EVENT_MAP_LOAD_SP_UI, void, char* map_name);

void init_map_hooks();
void revert_map_hooks();
