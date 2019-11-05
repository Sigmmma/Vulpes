/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#pragma once

#include <cstdint>

uint32_t datum_new(void* data);

uint32_t datum_new_at_index(void* data, uint32_t id);

uint32_t datum_delete(void* data, uint32_t id);
