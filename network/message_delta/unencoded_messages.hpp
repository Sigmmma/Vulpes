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

struct CtfUpdateHeader {
    uint32_t flag_swap_timer;
};

struct CtfUpdate {
    int32_t team_score[2];
    int32_t team_with_flag;
};

// This one leaves me uncertain.
struct OddballUpdate {
    PAD(4);
    int32_t team_score[16];
    int32_t player_score[16];
    PAD(64);
    int32_t is_holding_ball[16];
};

struct KingUpdate {
    PAD(0x1A4); // king score array?
    int32_t hill_id;
};

struct RaceUpdate {
    int32_t lap_completed_val;
    int32_t first_flag[16];
    int32_t lap_bit_vector[16];
    int32_t rally_flag;
    int32_t team_laps;
};

struct PlayerScoreUpdateHeader {
    int32_t player_id;
};

struct PlayerScoreUpdate {
    int16_t kills[4];
    int16_t assists[4];
    int16_t friendly_fire_kills;
    int16_t deaths;
    int16_t suicides;
    int16_t flag_grabs;
    int16_t flag_returns;
    int16_t flag_scores;
    int32_t special;
    bool is_odd_man_out;
    PAD(3);
    float speed_multiplier;
};

struct GameEngineChangeMode {
    int32_t mode; // Make an enum later.
};

struct GameEngineMapReset {
    PAD(1);
};

struct MultiplayerHudMessage {
    int32_t player_id;
    int32_t message;
    int32_t message_data;
};

struct MultiplayerSound {
    int32_t id;
};

struct TeamChange {
    int8_t player_id;
    int8_t new_team_id;
};

struct UnitDropCurrentWeapon {
    int32_t unit_id;
    int32_t weapon_id;
    bool immediate;
};

struct VehicleNew {
    MemRef tag_id;
    int32_t server_object_id;
    int16_t owner_team_id;
    PAD(2);
    int16_t owner_player_id;
    int32_t owner_object_id;
    int32_t weapon_object_id[4];
    Vec3d position;
    Vec3d forward; // might want to normalize these
    Vec3d up; // might want to normalize these
    Vec3d translational_velocity;
    Vec3d angular_velocity;
    int8_t current_baseline_id;
};

struct BipedNew {
    MemRef tag_id;
    int32_t server_object_id;
    int16_t owner_team_id;
    PAD(2);
    int16_t owner_player_id;
    PAD(2);
    int32_t owner_object_id;
    Vec3d position;
    Vec3d forward; // might want to normalize these
    Vec3d up; // might want to normalize these
    Vec3d translational_velocity;
    RGBFloat change_color[4];
    bool flashlight;
    PAD(3);
    float light_battery;
    int8_t current_baseline_id;
    PAD(3);
    int16_t grenade_counts[2];
    float health;
    float shield;
    bool shield_stun_ticks_greater_than_zero;
};

struct ProjectileNew {
    MemRef tag_id;
    int32_t server_object_id;
    int16_t owner_team_id;
    PAD(2);
    int16_t owner_player_id;
    PAD(2);
    int32_t owner_object_id;
    Vec3d position;
    Vec3d forward; // might want to normalize these
    Vec3d up; // might want to normalize these
    Vec3d translational_velocity;
    Vec3d angular_velocity;
    int8_t current_baseline_id;
};

struct EquipmentNew {
    MemRef tag_id;
    int32_t server_object_id;
    int16_t owner_team_id;
    PAD(2);
    int16_t owner_player_id;
    PAD(2);
    int32_t owner_object_id;
    uint32_t object_flags;
    Vec3d position;
    Vec3d forward; // might want to normalize these
    Vec3d up; // might want to normalize these
    Vec3d translational_velocity;
    Vec3d angular_velocity;
    int8_t current_baseline_id;
};

struct WeaponNew {
    MemRef tag_id;
    int32_t server_object_id;
    int16_t owner_team_id;
    PAD(2);
    int16_t owner_player_id;
    PAD(2);
    int32_t owner_object_id;
    uint32_t object_flags;
    Vec3d position;
    Vec3d forward; // might want to normalize these
    Vec3d up; // might want to normalize these
    Vec3d translational_velocity;
    int8_t current_baseline_id;
    PAD(1);
    int16_t magazine_totals[2];
    float age;
    int16_t magazine_loadeds[2];
};

struct GameSettingsUpdate {
    char name[0x80];
    char map_name[0x84];
    char variant[0xD8];
    int8_t unused_game_engine;
    int8_t max_players;
    int16_t difficulty;
    int16_t player_count;
    char players[0x200];
    int32_t network_game_random_seed;
    int32_t number_of_games_played;
    int32_t local_data;
};

// struct ParametersControl

struct LocalPlayerUpdate {
    int8_t sequence_numbers;
    int8_t last_completed_update_id;
    PAD(2);
    Vec3d position;
};



struct VulpesMessage {
    uint16_t payload_size;
    union {
        uint8_t  payload8[1000];
        uint16_t payload16[500];
        uint32_t payload32[250];
    };
}; static_assert(sizeof(VulpesMessage) == 1002);

#pragma pack(pop)
