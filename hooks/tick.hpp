#pragma once
#include "event.hpp"

typedef void (*EVENT_PRE_TICK)();
typedef void (*EVENT_TICK)();

std::vector<Event<EVENT_PRE_TICK>>* EVENT_PRE_TICK_list();
std::vector<Event<EVENT_TICK>>* EVENT_TICK_list();

void init_tick_hook();
void revert_tick_hook();
