/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#pragma once

#include <vulpes/memory/message_delta.hpp>

void send_vulpes_message(VulpesMessage* msg);

void handle_vulpes_message(VulpesMessage* msg);

void handle_hud_chat_vulpes_message(const wchar_t* msg);
