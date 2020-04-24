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

#include <vulpes/memory/signatures/signatures.hpp>

#include "object.hpp"

#include "generated/object_device.cpp"
#include "generated/object_item.cpp"
#include "generated/object_unit.cpp"
#include "generated/object.cpp"

GenericTable* device_groups_table() {
    return reinterpret_cast<GenericTable**>(
        sig_effect_table_refs())[8];
}

ObjectTable* object_table() {
    return **reinterpret_cast<ObjectTable***>(
        sig_object_table_ref());
}
