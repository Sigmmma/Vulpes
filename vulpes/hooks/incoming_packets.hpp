/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#pragma once

#include <vulpes/event.hpp>
#include <vulpes/memory/message_delta.hpp>

DEFINE_EVENT_HOOK(EVENT_RECEIVE_CHAT_MESSAGE, bool, HudChat*);

void init_incoming_packet_hooks();
void revert_incoming_packet_hooks();
