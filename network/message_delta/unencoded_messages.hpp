#pragma once
#include "enums.hpp"
#include "../../memory/types.hpp"
#include "../../memory/player.hpp"

#pragma pack(push, 1)

// Here follows structs that can be fed to the message_delta_encoder.
// These are the unpacked structs, the actual structs sent over the network are a lot smaller.
struct ObjectDeletion {
    int32_t network_id;
};

//Shared struct
struct ObjectUpdateHeader {
    int32_t network_id;
    int8_t baseline_id;
    int8_t message_index;
    bool update_baseline;
};

struct ObjectUpdateHeaderTimestamped : public ObjectUpdateHeader {
    uint8_t _pad;
    uint16_t timestamp; // I'm not sure what type this is yet.
};

// Encode ObjectUpdateHeader before this
struct ProjectileUpdate {
    Vec3d position;
    Vec3d translational_velocity;
};

// Encode ObjectUpdateHeader before this
struct EquipmentUpdate {
    Vec3d locality_reference_position;
    Vec3d translational_velocity;
    Vec3d angular_velocity;
};

// Encode ObjectUpdateHeader before this
struct WeaponUpdate {
    Vec3d locality_reference_position;
    Vec3d translational_velocity;
    int16_t magazine_0_rounds_total;
    int16_t magazine_1_rounds_total;
    float age; // Check if actually a float.
};

struct BipedUpdateHeader {
    int32_t network_id;
    int8_t baseline_id;
    int8_t message_index;
    bool update_baseline;
    bool force_shield_update;
};

// Encode BipedUpdateHeader before this
struct BipedUpdate {
    int16_t grenade_0_count;
    int16_t grenade_1_count;
    float body_vitality;
    float shield_vitality;
    bool shield_stun_ticks_greater_than_zero;
};

// Encode ObjectUpdateHeaderTimestamped before this
struct VehicleUpdate {
    bool at_rest;
    uint8_t _pad[3];
    Vec3d position;
    Vec3d translational_velocity;
    Vec3d angular_velocity;
    Vec3d forward;
    Vec3d up;
};

struct HudAdditem {
    MemRef item_definition_index;
    int8_t message_offset;
    int16_t quantity;
};

struct PlayerCreate {
    uint8_t player_id;
    uint8_t _pad[3];
    int32_t raw_player_index;//?
    int32_t translated_player_index;
    int32_t team_index;
};

struct PlayerSpawn {
    int32_t translated_player_index;
    int32_t network_id; // Object player will control
    int32_t team_index;
    int32_t parent_vehicle_network_id;
    int16_t vehicle_seat_index;
    int16_t _pad;
    int32_t weapon_object_network_ids[4];
    int16_t desired_weapon_index;
    int16_t powerup_durations[2];
};

// Seems like this could work for ai too.
struct PlayerExitVehicle {
    int32_t network_id;
    bool forced_exit;
};

struct PlayerSetActionResult {
    int32_t player_index;
    int32_t action_result_type;
    int32_t action_object_network_id;
    InteractionType action_seat_index;
    int32_t weapon_to_drop_as_result_of_swap;
};

// See what happens if we use the creation of this packet as the trigger to send our velocity override packet.
struct PlayerEffectStart {
    MemRef damage_definition;
    int32_t damage_owner_object_network_id;
    int32_t damage_flags;
    Vec3d direction;
    float scale;
    float total_damage;
};

struct UnitKill {
    int32_t network_id;
    bool should_cause_ping;
    bool died;
    bool feign_death;
    bool died_flying;
    bool ignore_hard_pings;
    bool force_hard_pings;
    bool should_allignment_vector_be_null;
    uint8_t _pad;
    int16_t damage_part;
    int16_t _pad2;
    float angle;
    Vec2d allignment_vector;
    int32_t respawn_timer;
};

struct ClientGameUpdateHeader {
    int32_t update_id;
};

// Encode ClientGameUpdateHeader first
struct ClientGameUpdate {
    uint32_t ticks_to_apply_update_to[2]; //?
    uint32_t control_flags; // figure out if these are the same
    Vec2d desired_facing;
    Vec2d digital_throttle;
    float primary_trigger;
    int16_t desired_weapon_id;
    int16_t desired_grenade_id;
    int16_t desired_zoom_level;
};

struct PlayerHandlePowerup {
    int8_t player_id;
    int8_t _pad[3];
    int16_t powerup_time;
    int16_t duration_in_ticks;
};

struct HudChat {
    HudChatType msg_type;
    int8_t _pad[3];
    int8_t player_id;
    int8_t _pad2[3];
    wchar_t* message;
};

struct SlayerScoreArray {
    struct SlayerScore{
        int32_t score;
        int32_t kills;
        int32_t assists;
        int32_t deaths;
    }player[16]; // Todo, confirm order.
};

struct SlayerUpdate {
    SlayerScoreArray team_score;
    SlayerScoreArray individual_score;
};

struct VulpesMessage {
    uint32_t payload_size;
    union {
        uint8_t  payload8[1024];
        uint32_t payload32[256];
    };
}; static_assert(sizeof(VulpesMessage) == 1028);

#pragma pack(pop)
