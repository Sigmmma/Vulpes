#pragma once
#include "definition.hpp"

__attribute__((cdecl))
void handle_vulpes_message(MessageDeltaHeader* header);

uintptr_t get_vulpes_message_definition();
