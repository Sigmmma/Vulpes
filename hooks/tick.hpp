#pragma once
#include "event.hpp"

typedef void (*EVENT_TICK)();

std::vector<Event<EVENT_TICK>>* EVENT_TICK_list();

void init_tick_hook();
void revert_tick_hook();
