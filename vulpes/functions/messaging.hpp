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

#pragma once

#include <cstdarg>

#include <vulpes/memory/types.hpp>

#include "message_delta.hpp"

// Clears console
void console_clear();

// Print a formatted string to the Halo console.
void vcprintf(const ARGBFloat& color, const char* format, va_list args);
void cprintf (const ARGBFloat& color, const char* format, ...);
// Versions of cprintf that have set colors.
void cprintf(const char* format, ...); // Gray
void cprintf_info (const char* format, ...); // Green
void cprintf_warn (const char* format, ...); // Yellow
void cprintf_error(const char* format, ...); // Red

// Prints a formatted string to a client's console.
// -1 means all.
void rprintf(int player_id, const char* format, ...);

// Print a formatted string to the terminal.
// If a seperate terminal window was opened we print to that.
// If we're a server we use the internal printing system
// so we don't screw with user input.
void tprintf(const char* format, ...);

// Send a formatted chat message to any or all clients.
// -1 means all.
void chatf(HudChatType type, int src_player, int dest_player,
           const char* format, ...);
