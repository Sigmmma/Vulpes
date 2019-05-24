/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
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
    bool does_not_operate_automatically : 1;
    bool machine_one_sided : 1;
    bool never_appears_locked : 1;
    bool opened_by_melee_attack : 1;
    bool _device_machine_pad_bits1 : 4;
    PAD(3);
    uint32_t ticks_since_started_opening;
    Vec3d elevator_position;
}; static_assert(sizeof(ObjectDeviceMachine) == SIZE_MACHINE);



class ObjectDeviceControl : public ObjectDevice {
public:
    bool usable_from_both_sides : 1;
    bool device_control_unused_bits1 : 4;
    bool device_control_pad_bits1 : 3;
    PAD(3);
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

Table* device_groups_table();
