/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <cstdio>
#include <cstring>
#include <string>

#include <vulpes/memory/gamestate/console.hpp>
#include <vulpes/memory/signatures.hpp>

#include "messaging.hpp"

typedef uint32_t (*FpReturnsInt)();

ConsoleOutput* console_new_line() {
    ConsoleGlobals* globals = console_globals();
    static auto console_line_new =
        reinterpret_cast<FpReturnsInt>(func_console_line_new());
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
    for (size_t i = 0; i < output->max_count; i++) {
        output->entries[i].text[0] = 0;
    }
}

typedef __attribute__((regparm(1)))void (*ConsoleToTerminalAndNetworkCall)(void*);

void vcprintf(const ARGBFloat& color, const char* format, va_list args) {
    static auto console_to_terminal_and_network =
        reinterpret_cast<ConsoleToTerminalAndNetworkCall>(
                func_console_to_terminal_and_network());
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
