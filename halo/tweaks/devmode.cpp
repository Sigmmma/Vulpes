/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

// NOP out all 6 bytes for devmode
Signature(false, sig_enabled_devmode,
    {0x74, 0x18, 0x85, 0xC0, 0x7E, 0x39});
