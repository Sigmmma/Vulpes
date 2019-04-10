#pragma once
#include <cstdint>
#include "types.hpp"

#include "object.hpp"
#include "actor.hpp"
#include "damage_effect.hpp"

enum class UnitAnimationState : int8_t {
    INVALID = -1,
    IDLE = 0,
    GESTURE,
    TURN_LEFT,
    TURN_RIGHT,
    MOVE_FRONT,
    MOVE_BACK,
    MOVE_LEFT,
    MOVE_RIGHT,
    STUNNED_FRONT,
    STUNNED_BACK,
    STUNNED_LEFT,
    STUNNED_RIGHT,
    SLIDE_FRONT,
    SLIDE_BACK,
    SLIDE_LEFT,
    SLIDE_RIGHT,
    READY,
    PUT_AWAY,
    AIM_STILL,
    AIM_MOVE,
    AIRBORNE,
    LAND_SOFT,
    LAND_HARD,
    UNKNOWN23,
    AIRBORNE_DEAD,
    LANDING_DEAD,
    SEAT_ENTER,
    SEAT_EXIT,
    CUSTOM_ANIMATION,
    IMPULSE,
    MELEE,
    MELEE_AIRBORNE,
    MELEE_CONTINUOUS,
    THROW_GRENADE,
    RESSURECT_FRONT,
    RESSURECT_BACK,
    FEEDING,
    SURPRISE_FRONT,
    SURPRISE_BACK,
    LEAP_START,
    LEAP_AIRBORNE,
    LEAP_MELEE,
    UNKNOWN42,
    BERSERK,
    YELO_SEAT_BOARDING,
    YELO_SEAT_EJECTING,
    YELO_MOUNTING,
    YELO_TRANSFORMING
};

enum class UnitReplacementAnimationState : int8_t {
    NONE = 0,
    DISARM,
    WEAPON_DROP,
    WEAPON_READY,
    WEAPON_PUT_AWAY,
    WEAPON_RELOAD1,
    WEAPON_RELOAD2,
    MELEE,
    THROW_GRENADE
};

enum class UnitOverlayAnimationState : int8_t {
    NONE = 0,
    FIRE1,
    FIRE2,
    CHARGED1,
    CHARGED2,
    CHAMBER1,
    CHAMBER2
};

enum class UnitThrowingGrenadeState : int8_t {
    NONE = 0,
    BEGIN,
    IN_HAND,
    RELEASED
};

enum class UnitBaseSeat : int8_t {
    ASLEEP = 0,
    ALERT,
    STAND,
    CROUCH,
    FLEE,
    FLAMING
};

enum class UnitBaseWeapon : int8_t {
    UNARMED = 0
};

enum class UnitSpeechPriority : int16_t {
    NONE = 0,
    IDLE,
    PAIN,
    TALK,
    COMMUNICATE,
    SHOUT,
    SCRIPT,
    INVOLUNTARY,
    EXLAIM,
    SCREAM,
    DEATH
};
enum class UnitScreamType : int16_t {
    FEAR = 0,
    ENEMY_GRENADE,
    PAIN,
    MAIMED_LIMB,
    MAIMED_HEAD,
    RESSURECTION
};

#pragma pack(push, 1)

struct UnitControlBits {
    bool crouch : 1;
    bool jump : 1;
    bool user1 : 1;
    bool user2 : 1;
    bool light : 1;
    bool exact_facing : 1;
    bool action : 1;
    bool melee : 1;

    bool look_dont_turn : 1;
    bool force_alert : 1;
    bool reload : 1;
    bool primary_trigger : 1;
    bool secondary_trigger : 1;
    bool grenade : 1;
    bool swap_weapon : 1;
    BITPAD(bool, 1);
};

struct UnitControlData {
    int8_t animation_state;
    int8_t aiming_speed;
    UnitControlBits bits;
    int16_t weapon_index;
    int16_t grenade_index;
    int16_t zoom_index;
    PAD(2);
    Vec3d throttle;
    float primary_trigger;
    Vec3d facing_vector;
    Vec3d aiming_vector;
    Vec3d looking_vector;
}; static_assert(sizeof(UnitControlData) == 0x40);

struct AnimationState {
    int16_t animation_id;
    int16_t frame_id;
};


class ObjectUnit : public Object {
public:
    MemRef actor;
    struct {
        MemRef actor;
        MemRef next_unit;
        MemRef prev_unit;
    } swarm;
    BITPAD(bool, 4); //unknown
    bool powerup_on : 1;
    bool powerup_additional : 1;
    bool controllable : 1;
    bool berserking : 1;
    BITPAD(bool, 8); //unknown
    BITPAD(bool, 3); //unknown
    bool unknown_bit19 : 1; // integrated_light_related
    bool will_not_drop_items : 1;
    bool unknown_bit21 : 1;
    bool can_blink : 1;
    bool impervious : 1; // prevents unit from being knocked around or playing ping animations
    bool suspended : 1;
    bool blind : 1;
    bool unknown_bit26 : 1; // when this is on, the integrated NV power increases. rate is 2x the speed it leaks when on
    // Wtf is an NV??
    bool possessed : 1;
    bool desires_flashlight_on : 1;
    bool desires_flashlight_off : 1;
    BITPAD(bool, 2);
    UnitControlBits control_bits;
    PAD(2);
    PAD(2);
    int8_t shield_sapping;
    int8_t base_seat_id;
    struct {
        int32_t ticks_remaining;
        UnitControlBits bits;
        PAD(2);
    } persistent_control;
    MemRef controlling_player;
    int16_t ai_effect_type; // enum
    int16_t emotion_animation_id;
    int32_t next_ai_effect_tick;
    Vec3d desired_facing_vector;
    Vec3d desired_aiming_vector;
    Vec3d aiming_vector;
    Vec3d aiming_velocity;
    Euler3d looking_angles;
    Vec3d looking_vector;
    Vec3d looking_velocity;
    Vec3d throttle;
    float primary_trigger;
    int8_t aiming_speed; // enum?
    int8_t melee_state; // enum, guess
    int8_t melee_timer; // guess
    int8_t ticks_until_flame_to_death;
    // also set to the same PersistentControlTicks value when an actor dies and they fire-wildely
    int8_t ping_animation_ticks_left;
    UnitThrowingGrenadeState grenade_state;
    int16_t _unknown20;
    int16_t _unknown21;
    PAD(2);
    int16_t grenade_projectile;
    struct UnitAnimationData {
        bool animation_bit0_unknown : 1;
        bool animation_bit1_unknown : 1;
        bool animation_bit2_unknown : 1;
        bool animation_bit3_unknown : 1;
        BITPAD(bool, 4);
        PAD(1);
        int16_t _unknown_some_animation_index_maybe;
        int16_t _unknown_some_animation_index;
        PAD(2); // Only set on initialization, never read afterwards.
        // animation graph unit indexes
        int8_t seat_id;
        int8_t seat_weapon_id;
        int8_t weapon_type_id;

        UnitAnimationState state;
        UnitReplacementAnimationState replacement_state;
        UnitOverlayAnimationState overlay_state;
        UnitAnimationState desired_state;
        UnitBaseSeat base_seat;
        int8_t emotion;
        PAD(1);
        AnimationState replacement_animation;
        AnimationState overlay_state_animation;
        AnimationState weapon_ik;
        bool update_look;           // Set when bounds below change
        bool update_aim;            //
        Rectangle2d looking_bounds;
        Rectangle2d aiming_bounds;
        PAD(8);
    } animation; static_assert(sizeof(UnitAnimationData) == 0x48);

    float ambient;
    float illumination;
    float mouth_factor;
    PAD(4);
    int16_t vehicle_seat_id;
    int16_t current_weapon_id;
    int16_t next_weapon_id;
    PAD(2);
    MemRef weapons[4];
    int32_t weapon_ready_ticks[4];
    MemRef equipment_id;
    int8_t current_grenade_id;
    int8_t next_grenade_id;
    uint8_t grenade_counts[2];
    // Opensauce is weird and has this defined as a unit_control
    // so it can be two extra grenade counts when unzoomed.
    uint8_t zoom_level;
    uint8_t desired_zoom_level;
    // End of opensauce shenanigans
    int8_t ticks_since_last_vehicle_speech;
    uint8_t aiming_change;
    MemRef powered_seats_riders[2];
    // these are related
    MemRef _unknown22;
    int32_t _some_tick_time;
    //
    int16_t encounter_id;
    int16_t squad_id;
    float powered_seats_power[2];
    float integrated_light_power;
    float integrated_light_toggle_power; // minimum power for a toggle?
    float integrated_night_vision_toggle_power;
    Vec3d seat_related[4]; //unknown
    float camo_power;
    float full_spectrum_vision_power; // gets updated, but not used.
    MemRef dialogue_definition;
    struct {
        struct UnitSpeech {
            UnitSpeechPriority priority;
            UnitScreamType scream_type;
            MemRef sound_tag;
            int16_t ticks; //unknown
            PAD(2);
            int32_t _unknown;
            AiCommunicationPacket ai_communication_info;
        }; static_assert(sizeof(UnitSpeech) == 0x30);
        UnitSpeech current;
        UnitSpeech next;
        int16_t _unknown0;
        int16_t _unknown1;
        int16_t _unknown2;
        int16_t _unknown3;
        int32_t _unknown4;
        bool _unknown6;
        bool _unknown7;
        bool _unknown8;
        PAD(1);
        int16_t _unknown9;
        int16_t _unknown10;
        int16_t _unknown11;
        int16_t _unknown12;
        int32_t _unknown13;
    } speech;
    struct {
        DamageCategory catagory;
        int16_t ai_ticks_until_handle;
        float amount;
        MemRef responsible_unit;
    }damage_result;
    MemRef object_flame_causer; // Object that caused flaming death
    float _unknown23;
    PAD(4);
    int32_t died_at_tick;
    int16_t feign_death_timer;
    bool camo_regrowth;
    PAD(1);
    float stun;
    int16_t stun_ticks;
    int16_t spree_count;
    int32_t spree_starting_time;
    struct UnitRecentDamage {
        int32_t tick;
        float damage;
        MemRef responsible_unit;
        MemRef responsible_player;
    } recent_damage[4];
    PAD(4);
    PAD(2); //Unused network bools
    uint8_t opensauce_zoom_level;
    uint8_t opensauce_desired_zoom_level;
    UnitControlData control_data;
    bool last_completed_client_update_valid;
    PAD(3);
    int32_t last_complted_client_update_id;
    PAD(3*4);
}; static_assert(sizeof(ObjectUnit) == SIZE_UNIT);

enum class BipedMovementState : int8_t {
    MOVING = 0,
    IDLE, // or turning
    GESTURING
};

class UnitBiped : public ObjectUnit {
public:
    bool airborne : 1;
    bool slipping : 1;
    bool absolute_movement : 1;
    bool no_collision : 1;
    bool passes_through_other_bipeds : 1;
    bool limping2 : 1;
    BITPAD(bool, 2); //unknown
    PAD(3); // unknown flags?
    // I expect these to be stun related
    int8_t landing_timer; // counts up whe biped lands, gets higher depending on height.
    int8_t landing_force; // instantly changes when landing. Depends on how hard the fall was.
    BipedMovementState movement_state;
    PAD(1);
    int32_t _biped_unknown3;
    uint32_t action_flags; // Something to do with walking and jumping
                    // maybe another set of control flags
    int32_t _biped_unknown4;
    Vec3d biped_position;
    int32_t walking_counter; //? Counts up when moving
    PAD(3*4); //unknown
    MemRef bump_object; //references the object that this biped last bumped into.
    int8_t ticks_since_last_bump;
    int8_t airborne_ticks;
    int8_t slipping_ticks; // counts up when hit by nade
    int8_t digital_throttle;
    int8_t jump_ticks;
    int8_t melee_ticks;
    int8_t melee_inflict_ticks;
    PAD(1);
    int16_t _biped_unknown2;
    PAD(2);
    float crouch_scale;
    float _biped_unknown1;
    Plane3d _biped_unknown_physics_related;
    PAD(2); //two unknown signed bytes
    struct BipedNetwork {
        bool baseline_valid;
        int8_t baseline_id;
        int8_t message_id;
        PAD(3);
        struct BipedNetworkDelta {
            int8_t grenade_counts[2];
            PAD(2);
            float body_vitality;
            float shield_vitality;
            bool shield_stun_ticks_greater_than_zero;
            PAD(3);
        }; static_assert(sizeof(BipedNetworkDelta) == 0x10);
        BipedNetworkDelta update_baseline;
        bool delta_valid;
        PAD(3);
        BipedNetworkDelta update_delta;
    } network;
}; static_assert(sizeof(UnitBiped) == SIZE_BIPED);



#pragma pack(pop)
