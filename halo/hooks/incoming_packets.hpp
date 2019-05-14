#pragma once
#include "../network/message_delta/unencoded_messages.hpp"
#include "event.hpp"

DEFINE_EVENT_HOOK(EVENT_RECEIVE_CHAT_MESSAGE, bool, HudChat*);

void init_incoming_packet_hooks();
void revert_incoming_packet_hooks();
