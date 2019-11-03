/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#pragma once
#include "object.hpp"
#include "../../actor.hpp"
#include "../../damage_effect.hpp"

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
    int8_t      animation_state;
    int8_t      aiming_speed;
    UnitControlBits bits;
    int16_t     weapon_index;
    int16_t     grenade_index;
    int16_t     zoom_index;
    PAD(2);
    Vec3d       throttle;
    float       primary_trigger;
    Vec3d       facing_vector;
    Vec3d       aiming_vector;
    Vec3d       looking_vector;
}; static_assert(sizeof(UnitControlData) == 0x40);

struct AnimationState {
    int16_t animation_id;
    int16_t frame_id;
};

struct UnitSpeech {
    UnitSpeechPriority priority;            // 388 // 3B8
    UnitScreamType  scream_type;            // 38A // 3BA
    MemRef          sound_tag;              // 38C // 3BC
    int16_t         ticks; //unknown        // 390 // 3A0
    int16_t         _unknown2;              // 392 // 3A2
    int32_t         _unknown;               // 394 // 3A4
    AiCommunicationPacket ai_communication_info;// 398 // 3A8
}; static_assert(sizeof(UnitSpeech) == 0x30);

class ObjectUnit : public Object {
public:
    // functions:
/*
    void speak(AiCommunicationType type,
               int16_t arg2,
               UnitSpeech new_speech);
*/
    // members:
    MemRef      actor;                          // 1F4
    struct {
        MemRef  actor;                          // 1F8
        MemRef  next_unit;                      // 1FC
        MemRef  prev_unit;                      // 200
    } swarm;
    struct {
        bool        _unknown_bit487         : 1; // checked by biped_speech_update
        BITPAD(bool, 3); //unknown                  // 204
        bool        powerup_on              : 1;
        bool        powerup_additional      : 1;
        bool        controllable            : 1;
        bool        berserking              : 1;
        BITPAD(bool, 8); //unknown
        BITPAD(bool, 3); //unknown
        bool        unknown_bit19           : 1; // integrated_light_related
        bool        will_not_drop_items     : 1;
        bool        unknown_bit21           : 1;
        bool        can_blink               : 1;
        bool        impervious              : 1; // prevents unit from being knocked around or playing ping animations
        bool        suspended               : 1;
        bool        blind                   : 1;
        bool        unknown_bit26           : 1; // when this is on, the integrated NV power increases. rate is 2x the speed it leaks when on
        // Wtf is an NV??
        bool        possessed               : 1;
        bool        desires_flashlight_on   : 1;
        bool        desires_flashlight_off  : 1;
        BITPAD(bool, 2);
    } unit_flags;
    UnitControlBits control_bits;               // 208
    PAD(2);
    PAD(2);                                     // 20C
    int8_t      shield_sapping;                 // 20E
    int8_t      base_seat_id;                   // 20F
    struct {
        int32_t         ticks_remaining;        // 210
        UnitControlBits bits;                   // 214
        PAD(2);
    }           persistent_control;
    MemRef      controlling_player;             // 218
    int16_t     ai_effect_type; // enum         // 21C
    int16_t     emotion_animation_id;           // 21E
    int32_t     next_ai_effect_tick;            // 220
    Vec3d       desired_facing_vector;          // 224
    Vec3d       desired_aiming_vector;          // 230
    Vec3d       aiming_vector;                  // 23C
    Vec3d       aiming_velocity;                // 248
    Euler3d     looking_angles;                 // 254
    Vec3d       looking_vector;                 // 260
    Vec3d       looking_velocity;               // 26C
    Vec3d       throttle;                       // 278
    float       primary_trigger;                // 284
    int8_t      aiming_speed; // enum?          // 288
    int8_t      melee_state; // enum, guess     // 289
    int8_t      melee_timer; // guess           // 28A
    int8_t      ticks_until_flame_to_death;     // 28B
    // also set to the same PersistentControlTicks value when an actor dies and they fire-wildely
    int8_t      ping_animation_ticks_left;      // 28C
    UnitThrowingGrenadeState grenade_state;     // 28D
    int16_t     _unknown20;                     // 28E
    int16_t     _unknown21;                     // 290
    PAD(2);                                     // 292
    MemRef     grenade_projectile;              // 294
    struct UnitAnimationData {
        struct {
            bool    animation_bit0_unknown : 1;     // 298
            bool    animation_bit1_unknown : 1;
            bool    animation_bit2_unknown : 1;
            bool    animation_bit3_unknown : 1;
            BITPAD(bool, 4);
            PAD(1);
        } flags;
        int16_t _unknown_some_animation_index_maybe; // 29A
        int16_t _unknown_some_animation_index;       // 29C
        PAD(2); // Only set on initialization, never read afterwards.//29C
        // animation graph unit indexes
        int8_t  seat_id;                             // 2A0
        int8_t  seat_weapon_id;                      // 2A1
        int8_t  weapon_type_id;                      // 2A2

        UnitAnimationState              state;              // 2A3
        UnitReplacementAnimationState   replacement_state;  // 2A4
        UnitOverlayAnimationState       overlay_state;      // 2A5
        UnitAnimationState              desired_state;      // 2A6
        UnitBaseSeat                    base_seat;          // 2A7
        int8_t                          emotion;            // 2A8
        PAD(1);
        AnimationState  replacement_animation;              // 2AA
        AnimationState  overlay_state_animation;            // 2AE
        AnimationState  weapon_ik;                          // 2B2
        bool            update_look; // Set when bounds below change // 2B6
        bool            update_aim;                         // 2B7
        Rectangle2d     looking_bounds;                     // 2B8
        Rectangle2d     aiming_bounds;                      // 2C8
        PAD(8);                                             // 2D8
    } animation; static_assert(sizeof(UnitAnimationData) == 0x48);

    float       ambient;                            // 2E0
    float       illumination;                       // 2E4
    float       mouth_factor;                       // 2E8
    PAD(4);
    int16_t     vehicle_seat_id;                    // 2F0
    int16_t     current_weapon_id;                  // 2F2
    int16_t     next_weapon_id;                     // 2F4
    PAD(2);
    MemRef      weapons[4];                         // 2F8
    int32_t     weapon_ready_ticks[4];              // 308
    MemRef      equipment_id;                       // 318
    int8_t      current_grenade_id;                 // 31C
    int8_t      next_grenade_id;                    // 31D
    uint8_t     grenade_counts[2];                  // 31E
    // Opensauce is weird and has this defined as a union
    // so it can be two extra grenade counts when unzoomed.
    uint8_t     zoom_level;                         // 320
    uint8_t     desired_zoom_level;                 // 321
    // End of opensauce shenanigans
    int8_t      ticks_since_last_vehicle_speech;    // 322
    uint8_t     aiming_change;                      // 323
    MemRef      powered_seats_riders[2];            // 324
    // these are related
    MemRef      _unknown22;                         // 32C
    int32_t     _some_tick_time;                    // 330
    //
    int16_t     encounter_id;                       // 334
    int16_t     squad_id;                           // 336
    float       powered_seats_power[2];             // 338
    float       integrated_light_power;             // 340
    float       integrated_light_toggle_power;      // 344 // minimum power for a toggle?
    float       integrated_night_vision_toggle_power;//348
    Vec3d       seat_related[4]; //unknown          // 34C
    float       camo_power;                         // 37C
    float       full_spectrum_vision_power;         // 380 // gets updated, but not used.
    MemRef      dialogue_definition;                // 384
    struct UnitSpeechData {
        UnitSpeech  current;
        UnitSpeech  next;
        int16_t     _unknown0;                      // 3E8 speech_ticks_left?
        int16_t     _unknown1;                      // 3EA speech_ticks_left?
        int16_t     _unknown2;                      // 3EC speech_ticks_left?
        int16_t     _unknown3;                      // 3EE
        int32_t     _unknown4;                      // 3F0
        bool        _unknown6;                      // 3F4
        bool        _unknown7;                      // 3F5
        bool        _unknown8;                      // 3F6
        PAD(1);
        int16_t     _unknown9;                      // 3F8
        int16_t     _unknown10;                     // 3FA
        int16_t     _unknown11;                     // 3FC copied from current._unknown2
        int16_t     _unknown12;                     // 3FE
        int32_t     _unknown13;                     // 400 set to -1 if _unknown10 becomes 0
    } speech; static_assert(sizeof(speech) == sizeof(UnitSpeech)*2 + 0x1C);
    struct {
        DamageCategory  catagory;                   // 404
        int16_t         ai_ticks_until_handle;      // 406
        float           amount;                     // 408
        MemRef          responsible_unit;           // 40C
    }           damage_result;
    MemRef      object_flame_causer;                // 410 // Object that caused flaming death
    float       _unknown23;                         // 414
    PAD(4);
    int32_t     died_at_tick;                       // 41C
    int16_t     feign_death_timer;                  // 420
    bool        camo_regrowth;                      // 422
    PAD(1);
    float       stun;                               // 424
    int16_t     stun_ticks;                         // 428
    int16_t     spree_count;                        // 42A
    int32_t     spree_starting_time;                // 42C
    struct UnitRecentDamage {
        int32_t tick;                               // 430 // 440 // 450 // 460
        float   damage;                             // 434 // 444 // 454 // 464
        MemRef  responsible_unit;                   // 438 // 448 // 458 // 468
        MemRef  responsible_player;                 // 43C // 44C // 45C // 46C
    } recent_damage[4];
    PAD(4);                                         // 470
    PAD(2); //Unused network bools                  // 474
    uint8_t     opensauce_zoom_level;               // 476
    uint8_t     opensauce_desired_zoom_level;       // 477
    UnitControlData control_data;                   // 478
    bool        last_completed_client_update_valid; // 4B8
    PAD(3);
    int32_t     last_completed_client_update_id;    // 4BC
    PAD(3*4);
}; static_assert(sizeof(ObjectUnit) == SIZE_UNIT);

enum class BipedMovementState : int8_t {
    MOVING = 0,
    IDLE, // or turning
    GESTURING
};

class UnitBiped : public ObjectUnit {
public:
    struct {
        bool        airborne : 1; // 4CC
        bool        slipping : 1;
        bool        absolute_movement : 1;
        bool        no_collision : 1;
        bool        passes_through_other_bipeds : 1;
        bool        limping2 : 1;
        BITPAD(bool, 2); //unknown
        PAD(3); // unknown flags?
    } biped_flags;
    // I expect these to be stun related
    int8_t      landing_timer; // counts up whe biped lands, gets higher depending on height.
    int8_t      landing_force; // instantly changes when landing. Depends on how hard the fall was.
    BipedMovementState movement_state;
    PAD(1);
    int32_t     _biped_unknown3;
    uint32_t    action_flags; // Something to do with walking and jumping
                    // maybe another set of control flags
    int32_t     _biped_unknown4;
    Vec3d       biped_position;
    int32_t     walking_counter; //? Counts up when moving
    PAD(3*4); //unknown
    MemRef      bump_object; //references the object that this biped last bumped into.
    int8_t      ticks_since_last_bump;
    int8_t      airborne_ticks;
    int8_t      slipping_ticks; // counts up when hit by nade
    int8_t      digital_throttle;
    int8_t      jump_ticks;
    int8_t      melee_ticks;
    int8_t      melee_inflict_ticks;
    PAD(1);
    int16_t     _biped_unknown2;
    PAD(2);
    float       crouch_scale;
    float       _biped_unknown1;
    Plane3d     _biped_unknown_physics_related;
    PAD(2); //two unknown signed bytes
    struct BipedNetwork {
        bool    baseline_valid;
        int8_t  baseline_id;
        int8_t  message_id;
        PAD(3);
        struct BipedNetworkDelta {
            int8_t  grenade_counts[2];
            PAD(2);
            float   body_vitality;
            float   shield_vitality; // Actual shield divided by 3
            bool    shield_stun_ticks_greater_than_zero;
            PAD(3);
        }; static_assert(sizeof(BipedNetworkDelta) == 0x10);
        BipedNetworkDelta update_baseline;
        bool    delta_valid;
        PAD(3);
        BipedNetworkDelta update_delta;
    } network;
}; static_assert(sizeof(UnitBiped) == SIZE_BIPED);

class UnitVehicle : public ObjectUnit {
public:
    struct {
        bool    _vehicle_unknown0 : 1;          // 4CC
        bool    hovering : 1;
        bool    crouched : 1;
        bool    jumping : 1;
        bool    _vehicle_unknown1 : 1;
        bool    _vehicle_unknown2 : 3;
        PAD(1);
    } vehicle_flags;
    PAD(2); // unknown int16                // 4CD
    PAD(4); //unknown set of bytes          // 4D0
    float   speed;                          // 4D4
    float   slide;                          // 4D8
    float   turn;                           // 4DC
    float   tire_position;                  // 4E0
    float   thread_position_left;           // 4E4
    float   thread_position_right;          // 4E8
    float   hover;                          // 4EC
    float   thrust;                         // 4F0
    int8_t  suspension_states[8];           // 4F4
    Vec3d   hover_position;                 // 4FC
    Vec3d   _vehicle_unknown3;              // 508
    Vec3d   _vehicle_unknown4;              // 514
    int32_t _vehicle_unknown5;              // 520
    struct VehicleNetwork {
        bool    time_valid;                 // 524
        bool    baseline_valid;             // 525
        int8_t  baseline_id;                // 526
        int8_t  message_id;                 // 527
        struct VehicleNetworkData {
            bool    at_rest;
            PAD(3);
            Vec3d   position;
            Vec3d   transitional_velocity;
            Vec3d   angular_velocity;
            Vec3d   forward;
            Vec3d   up;
        }; static_assert(sizeof(VehicleNetworkData) == 0x40);
        VehicleNetworkData  update_baseline;// 528
        bool    delta_valid; //? A comment reads 'unused'
        PAD(3);
        VehicleNetworkData  update_delta;   // 56C
        int32_t last_moved_at_tick; // -1 if the vehicle is occupied  // 5AC
        // For all gametypes except race this is the vehicle_block id
        // For race this is the netgame_flag
        int16_t scenario_respawn_id;        // 5B0
        PAD(2);
        // Only used to check if the vehicle should respawn it seems.
        Vec3d   respawn_position; // unknown function. // 578
    }network;
}; static_assert(sizeof(UnitVehicle) == SIZE_VEHICLE);

#pragma pack(pop)
