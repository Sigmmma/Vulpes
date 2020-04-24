/*
 * This file is part of Vulpes, an extension of Halo Custom Edition's capabilities.
 * Copyright (C) 2019-2020 gbMichelle (Michelle van der Graaf)
 *
 * Vulpes is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, version 3.
 *
 * Vulpes is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * long with Vulpes.  If not, see <https://www.gnu.org/licenses/agpl-3.0.en.html>
 */

#include <cstdio>
#include <cstring>
#include <string>

#include <vulpes/memory/persistent/console.hpp>
#include <vulpes/memory/signatures/signatures.hpp>

#include "messaging.hpp"

typedef uint32_t (*FpReturnsInt)();

ConsoleOutput* console_new_line() {
    ConsoleGlobals* globals = console_globals();
    static auto console_line_new =
        reinterpret_cast<FpReturnsInt>(sig_func_console_line_new());
    MemRef line;
    line.raw = console_line_new();

    if (line.raw != 0xFFFFFFFF) {
        ConsoleOutput* output = &globals->output->entries[line.id.local];
        output->fade_frames = 0;
        return output;
    } else {
        return NULL;
    }
}

void console_clear() {
    // This is not the best way to do it. But it serves the purpose.
    // We're nulling out all the first chars so Halo doesn't actually see text!
    ConsoleOutputTable* output = console_output_table();
    for (size_t i = 0; i < output->max_elements; i++) {
        output->entries[i].text[0] = 0;
    }
}

typedef __attribute__((regparm(1)))void (*ConsoleToTerminalAndNetworkCall)(void*);

void vcprintf(const ARGBFloat& color, const char* format, va_list args) {
    static auto console_to_terminal_and_network =
        reinterpret_cast<ConsoleToTerminalAndNetworkCall>(
                sig_func_console_to_terminal_and_network());
    ConsoleGlobals* globals = console_globals();
    if (globals->initialized) {
        ConsoleOutput* output = console_new_line();
        if (output) {
            vsnprintf(&output->text[0], 255, format, args);
            output->color = color;
            output->tab_stops = strstr(output->text, "|t") != NULL;
            if (console_to_terminal_and_network) {
                console_to_terminal_and_network(&output->text);
            }
        }
    }
}

void cprintf(const ARGBFloat& color, const char* format, ...) {
    va_list args;
    va_start(args, format);
    vcprintf(color, format, args);
    va_end(args);
}

void cprintf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vcprintf(ARGBFloat(1.0, 0.7, 0.7, 0.7), format, args);
    va_end(args);
}

void cprintf_info (const char* format, ...) {
    va_list args;
    va_start(args, format);
    vcprintf(ARGBFloat(1.0, 0.0, 1.0, 0.0), format, args);
    va_end(args);
}

void cprintf_warn (const char* format, ...) {
    va_list args;
    va_start(args, format);
    vcprintf(ARGBFloat(1.0, 1.0, 0.835, 0.0), format, args);
    va_end(args);
}

void cprintf_error(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vcprintf(ARGBFloat(1.0, 1.0, 0.0, 0.0), format, args);
    va_end(args);
}

void rprintf(int player_id, const char* format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[80+16];
    RconResponse message;
    memset(&message.text[0], 0, 80);
    vsnprintf(&message.text[0], 80, format, args);
    uint32_t size = mdp_encode_stateless_iterated(buffer, RCON_RESPONSE, &message);
    if (player_id < 0) {
        send_delta_message_to_all(&buffer, size, true, true, false, true, 2);
    } else if (player_id < 16) {
        send_delta_message_to_player(player_id, &buffer, size, true, true, false, true, 2);
    }
    va_end(args);
}

void chatf(HudChatType type, int src_player, int dest_player,
           const char* format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[512+16];
    vsnprintf(&buffer[0], 256, format, args);
    wchar_t output[256];
    std::mbstowcs(output, buffer, 256);
    HudChat message(type, src_player, &output[0]);
    uint32_t size = mdp_encode_stateless_iterated(buffer, HUD_CHAT, &message);
    if (dest_player < 0) {
        send_delta_message_to_all(&buffer, size, true, true, false, true, 3);
    } else if (dest_player < 16) {
        send_delta_message_to_player(dest_player, &buffer, size, true, true, false, true, 3);
    }
    va_end(args);
}
