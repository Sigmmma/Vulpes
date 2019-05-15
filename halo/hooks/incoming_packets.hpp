#pragma once
#include "../memory/message_delta.hpp"
#include "../event.hpp"

DEFINE_EVENT_HOOK(EVENT_RECEIVE_CHAT_MESSAGE, bool, HudChat*);

void init_incoming_packet_hooks();
void revert_incoming_packet_hooks();
