/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#pragma once
#include "../memory/types.hpp"
#include <cstdarg>

// Print a formatted string to the Halo console.
void vcprintf(const ARGBFloat& color, const char* format, va_list args);
void cprintf (const ARGBFloat& color, const char* format, ...);
// Versions of cprintf that have set colors.
void cprintf(const char* format, ...); // Gray
void cprintf_info (const char* format, ...); // Green
void cprintf_warn (const char* format, ...); // Yellow
void cprintf_error(const char* format, ...); // Red

// Prints a formatted string to a client's console.
void rprintf(int player_id, const char* format, ...);
void rprintf_all(const char* format, ...);

// Print a formatted string to the terminal.
// If a seperate terminal window was opened we print to that.
// If we're a server we use the internal printing system
// so we don't screw with user input.
void tprintf(const char* format, ...);