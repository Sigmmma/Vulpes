#pragma once
#include "definition.hpp"
#include "unencoded_messages.hpp"
/*
__attribute__((cdecl))
void handle_vulpes_message(MessageDeltaHeader* header);

uintptr_t get_vulpes_message_definition();
*/
void send_vulpes_message(VulpesMessage* msg);

void handle_vulpes_message(VulpesMessage* msg);

void handle_hud_chat_vulpes_message(const wchar_t* msg);
