#pragma once
#include "object.hpp"

#pragma pack(push, 1)

class ObjectItem : public Object {
public:
    uint32_t flags;
    int16_t ticks_until_detonation;
    struct {
        int16_t surface_id;
        int16_t reference_id; //bsp_reference_id
    } bsp_collision;
    PAD(2);
    MemRef dropped_by_unit; // Set when a unit that held this item drops it.
    int32_t last_update_tick;
    struct {
        MemRef object;
        Vec3d object_position;
    }object_collision;
    Vec3d _unknown_collision_position; //? My guesses without checking it yet.
    Euler2d _unknown_collision_angle; //?
}; static_assert(sizeof(ObjectItem) == SIZE_ITEM);



class ObjectGarbage : public ObjectItem {
public:
    int16_t ticks_until_garbage_collection;
    PAD(2);
    PAD(4*5);
}; static_assert(sizeof(ObjectGarbage) == SIZE_GARBAGE);



class ObjectWeapon : public ObjectItem {
public:
    enum WeaponState : int8_t {
        IDLE,
        FIRE1,      FIRE2,
        CHAMBER1,   CHAMBER2,
        RELOAD1,    RELOAD2,
        CHARGED1,   CHARGED2,
        READY,
        PUT_AWAY
    };

    uint32_t flags;
    uint16_t owner_unit_flags;
    PAD(2);
    float primary_trigger;
    WeaponState weapon_state;
    PAD(1);
    int16_t ready_ticks;
    float heat;
    float age;
    float illumination_fraction;
    float integrated_light_power;
    PAD(4);
    MemRef tracked_object;
    PAD(8);
    int16_t alt_shots_loaded;
    PAD(2);
    struct WeaponTrigger{
        enum WeaponTriggerState : int8_t {
            IDLE,
            UNKNOWN1, //Alternate shot/overload?
            CHARGING,
            CHARGED,
            UNKNOWN4, //Locked related (tracking)
            UNKNOWN5, // tracking related
            SPEWING,
            FROZEN_WHILE_TRIGGERED,
            FROZEN_TIMED
        };
        int8_t idle_time;
        WeaponTriggerState state;
        int16_t ticks;
        uint32_t _unknown_flags;
        int16_t _firing_effect_related[3];
        int16_t rounds_since_last_tracer;
        float rate_of_fire;
        float ejection_port_recovery_time;
        float illumination_recovery_time;
        float projectile_error_related;
        MemRef charing_effect;
        int8_t network_delay_ticks;
        PAD(3);
    }triggers[2]; static_assert(sizeof(WeaponTrigger) == 0x28);

    struct WeaponMagazine {
        enum WeaponMagazineState : int16_t {
            IDLE,
            CHAMBERING_START,
            CHAMBERING_FINISH,
            CHAMBERING
        };
        WeaponMagazineState state;
        int16_t reload_ticks_remaining;
        int16_t reload_ticks;
        int16_t rounds_unloaded;
        int16_t rounds_loaded;
        int16_t rounds_left_to_recharge; //number of rounds left to apply to rounds_loaded (based on tag's rounds_recharged)
        int16_t _unknown; //maybe an enum.
        int16_t _unknown2; // possibly padding
    }magazines[2]; static_assert(sizeof(WeaponMagazine) == 0x10);

    int32_t last_trigger_fire_tick;

    struct WeaponReloadStartData {
        int16_t total_rounds[2];
        int16_t loaded_rounds[2];
    }reload_starting_point; static_assert(sizeof(WeaponReloadStartData) == 8);

    PAD(4);

    struct WeaponNetwork {
        bool baseline_valid;
        int8_t baseline_index;
        int8_t message_index;
        PAD(1);

        struct WeaponNetworkData {
            Vec3d position;
            Vec3d transitional_velocity;
            Vec3d angular_velocity; //unused
            int16_t magazine_rounds_total[2];
            float age;
        }; static_assert(sizeof(WeaponNetworkData) == 0x2C);
        WeaponNetworkData update_baseline;
        bool delta_valid; // Maybe?
        PAD(3);
        WeaponNetworkData update_delta;
    } network;
}; static_assert(sizeof(ObjectWeapon) == SIZE_WEAPON);



class ObjectEquipment : public ObjectItem {
public:
    struct EquipmentNetwork {
        PAD(6*4); //Opensauce uses these padding bytes
        bool baseline_valid;
        int8_t baseline_index;
        int8_t message_index;
        PAD(1);
        struct EquipmentNetworkData {
            Vec3d position;
            Vec3d transitional_velocity;
            Vec3d angular_velocity;
        }; static_assert(sizeof(EquipmentNetworkData) == 0x24 );
        EquipmentNetworkData update_baseline;
        bool delta_valid; // Unsure
        PAD(3);
        EquipmentNetworkData update_delta;
    } network;
}; static_assert(sizeof(ObjectEquipment) == SIZE_EQUIPMENT);

class ObjectProjectile : public ObjectItem {
    bool tracer : 1;
    bool _projectile_unknown_bit : 1;
    bool attached : 1;
    bool _projectile_unknown_bit2 : 5;
    PAD(3); //possibly more bits
    int16_t action_enum;
    int16_t material_id; // unconfirmed.
    MemRef source_unit;
    MemRef target_object;
    int32_t contrail_attachment_block_id;
    float time_remaining; // To target or to 0.0? What?
    float arming_rate; // related to detonation coundown timer
    float _unknown_proj_float1;
    float _unknown_proj_float2; // related to arming time
    float distance_travelled;
    Vec3d transitional_velocity;
    float water_damage_upper_bound;
    Vec3d angular_velocity;
    Euler2d _unknown_euler;
    struct ProjectileNetwork {
        bool _unknown;
        bool baseline_valid;
        int8_t baseline_index;
        int8_t message_index;
        struct ProjectileNetworkData {
            Vec3d position;
            Vec3d transitional_velocity;
        }; static_assert(sizeof(ProjectileNetworkData) == 0x18 );
        ProjectileNetworkData update_baseline;
        bool delta_valid;
        PAD(3);
        ProjectileNetworkData update_delta;
    } network;
}; static_assert(sizeof(ObjectProjectile) == SIZE_PROJECTILE);

#pragma pack(pop)
