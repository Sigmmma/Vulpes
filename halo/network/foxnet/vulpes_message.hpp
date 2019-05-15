#pragma once
#include "../../memory/message_delta.hpp"

void send_vulpes_message(VulpesMessage* msg);

void handle_vulpes_message(VulpesMessage* msg);

void handle_hud_chat_vulpes_message(const wchar_t* msg);
