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

#include "object.hpp"

#pragma pack(push, 1)

class ObjectDevice : public Object {
public:
    bool position_reversed : 1;
    bool not_usable_from_any_side : 1;
    bool _device_pad_bits1 : 6;
    PAD(3);

    struct {
        int16_t device_group_id;
        PAD(2);
        float value;
        float change;
    }power,
    position;

    bool one_sided : 1;
    bool operates_automatically : 1;
    bool _device_pad_bits2 : 6;
    PAD(3);
}; static_assert(sizeof(ObjectDevice) == SIZE_DEVICE);



class ObjectDeviceMachine : public ObjectDevice {
public:
    struct {
        bool does_not_operate_automatically : 1;
        bool machine_one_sided : 1;
        bool never_appears_locked : 1;
        bool opened_by_melee_attack : 1;
        BITPAD(bool, 4);
        PAD(3);
    } device_flags;
    uint32_t ticks_since_started_opening;
    Vec3d elevator_position;
}; static_assert(sizeof(ObjectDeviceMachine) == SIZE_MACHINE);



class ObjectDeviceControl : public ObjectDevice {
public:
    struct {
        bool usable_from_both_sides : 1;
        bool device_control_unused_bits1 : 4;
        BITPAD(bool, 3);
        PAD(3);
    } device_control_flags;
    int16_t custom_name_id;
    PAD(2);
}; static_assert(sizeof(ObjectDeviceControl) == SIZE_CONTROL);



class ObjectDeviceLightFixture : public ObjectDevice {
public:
    RGBFloat light_color;
    float light_intensity;
    float light_falloff_angle;
    float light_cutoff_angle;
}; static_assert(sizeof(ObjectDeviceLightFixture) == SIZE_LIGHT_FIXTURE);

#pragma pack(pop)

GenericTable* device_groups_table();
