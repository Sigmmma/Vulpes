#pragma once
#include "event.hpp"

DEFINE_EVENT_HOOK(EVENT_PRE_TICK, void);
DEFINE_EVENT_HOOK(EVENT_TICK, void);

void init_tick_hook();
void revert_tick_hook();
