/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#pragma once

#include <vulpes/memory/gamestate/player.hpp>

#include "types.hpp"

enum MessageDeltaType : int8_t {
    NONE                               = -1,
    OBJECT_DELETION                    = 0,
    PROJECTILE_UPDATE                  = 1,
    EQUIPMENT_UPDATE                   = 2,
    WEAPON_UPDATE                      = 3,
    BIPED_UPDATE                       = 4,
    VEHICLE_UPDATE                     = 5,
    HUD_ADD_ITEM                       = 6,
    PLAYER_CREATE                      = 7,
    PLAYER_SPAWN                       = 8,
    PLAYER_EXIT_VEHICLE                = 9,
    PLAYER_SET_ACTION_RESULT           = 0xA,
    PLAYER_EFFECT_START                = 0xB,
    UNIT_KILL                          = 0xC,
    CLIENT_GAME_UPDATE                 = 0xD,
    PLAYER_HANDLE_POWERUP              = 0xE,
    HUD_CHAT                           = 0xF,
    SLAYER_UPDATE                      = 0x10,
    CTF_UPDATE                         = 0x11,
    ODDBALL_UPDATE                     = 0x12,
    KING_UPDATE                        = 0x13,
    RACE_UPDATE                        = 0x14,
    PLAYER_SCORE_UPDATE                = 0x15,
    GAME_ENGINE_CHANGE_MODE            = 0x16,
    GAME_ENGINE_MAP_RESET              = 0x17,
    MULTIPLAYER_HUD_MESSAGE            = 0x18,
    MULTIPLAYER_SOUND                  = 0x19,
    TEAM_CHANGE                        = 0x1A,
    UNIT_DROP_CURRENT_WEAPON           = 0x1B,
    VEHICLE_NEW                        = 0x1C,
    BIPED_NEW                          = 0x1D,
    PROJECTILE_NEW                     = 0x1E,
    EQUIPMENT_NEW                      = 0x1F,
    WEAPON_NEW                         = 0x20,
    GAME_SETTINGS_UPDATE               = 0x21,
    PARAMETERS_PROTOCOL                = 0x22,
    LOCAL_PLAYER_UPDATE                = 0x23,
    LOCAL_PLAYER_VEHICLE_UPDATE        = 0x24,
    REMOTE_PLAYER_ACTION_UPDATE        = 0x25,
    SUPER_REMOTE_PLAYERS_ACTION_UPDATE = 0x26,
    REMOTE_PLAYER_POSITION_UPDATE      = 0x27,
    REMOTE_PLAYER_VEHICLE_UPDATE       = 0x28,
    REMOTE_PLAYER_TOTAL_UPDATE_BIPED   = 0x29,
    REMOTE_PLAYER_TOTAL_UPDATE_VEHICLE = 0x2A,
    WEAPON_START_RELOAD                = 0x2B,
    WEAPON_AMMO_PICKUP_MID_RELOAD      = 0x2C,
    WEAPON_FINISH_RELOAD               = 0x2D,
    WEAPON_CANCEL_RELOAD               = 0x2E,
    NETGAME_EQUIPMENT_NEW              = 0x2F,
    PROJECTILE_DETONATE                = 0x30,
    ITEM_ACCELERATE                    = 0x31,
    DAMAGE_DEALT                       = 0x32,
    PROJECTILE_ATTACH                  = 0x33,
    CLIENT_TO_SERVER_PONG              = 0x34,
    SUPER_PING_UPDATE                  = 0x35,
    SV_MOTD                            = 0x36,
    RCON_REQUEST                       = 0x37,
    RCON_RESPONSE                      = 0x38,

    DEFAULT_MESSAGE_TYPE_COUNT = 0x39,

    VULPES_MSG = 0x39,

    NEW_MESSAGE_TYPE_COUNT = 0x3A
};

enum class MessageDeltaMode : int32_t {
    STATELESS = 0,
    INCREMENTAL = 1
};

enum class HudChatType : int8_t {
    NONE    = -1,
    ALL     = 0,
    TEAM    = 1,
    VEHICLE = 2,
    SERVER  = 3, // Phasor?
    PRIVATE = 4, // Phasor?
    HAC     = 5, // Sapp uses this channel to communicate with hac2.
    HCN     = 6, // Another mod uses this channel to communicate with hac2.
    VULPES  = 17  // The channel we use to communicate.
};

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
    PAD(1);
};

struct ObjectUpdateHeaderTimestamped : public ObjectUpdateHeader {
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
    PAD(3);
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
    PAD(3);
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
    PAD(2);
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
    PAD(1);
    int16_t damage_part;
    PAD(2);
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
    PAD(3);
    int16_t powerup_time;
    int16_t duration_in_ticks;
};

struct HudChat {
    HudChatType msg_type;
    PAD(3);
    int8_t player_id;
    PAD(3);
    wchar_t* message;
    HudChat(){}
    HudChat(HudChatType type, int8_t player, wchar_t* text)
        {msg_type=type; player_id=player; message=text;}
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

struct EquipmentNew : public ProjectileNew {
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
    int8_t sequence_number;
    int8_t last_completed_update_id;
    PAD(2);
    Vec3d position;
};

struct LocalPlayerVehicleUpdate {
    int8_t sequence_number;
    int8_t last_completed_update_id;
    PAD(2);
    int32_t vehicle_id;
    Vec3d position;
    Vec3d translational_velocity;
    Vec3d angular_velocity;
    Vec3d forward;
    Vec3d up;
};

struct RemotePlayerActionUpdateHeader {
    int32_t player_id;
    int8_t update_id;
    int8_t baseline_id;
};

struct RemotePlayerActionUpdate {
    uint32_t ticks_to_apply_update_to;
    uint32_t control_flags;
    PAD(8);
    float forward_throttle;
    float sideways_throttle;
    float primary_trigger;
    int16_t desired_weapon_id;
    int16_t desired_grenade_id;
    PAD(4);
    Vec3d facing;
};

struct RemotePlayerPositionUpdateHeader {
    int32_t player_id;
    int8_t update_id;
    int8_t sequence_number;
    PAD(2);
};

struct RemotePlayerPositionUpdate {
    Vec3d locality_reference_position;
};

struct RemotePlayerVehicleUpdate {
    int32_t vehicle_id;
    Vec3d position;
    Vec3d translational_velocity;
    Vec3d angular_velocity;
    Vec3d forward;
    Vec3d up;
};

struct RemotePlayerTotalUpdateBipedHeader : public RemotePlayerPositionUpdateHeader {
    int8_t sequence_number;
};

struct RemotePlayerTotalUpdateBiped : public RemotePlayerActionUpdate {
    Vec3d position;
};

struct RemotePlayerTotalUpdateVehicle {
    RemotePlayerActionUpdate player;
    RemotePlayerVehicleUpdate vehicle;
};

struct WeaponStartReload {
    int32_t weapon_id;
    int16_t magazine_id;
    int16_t total_rounds;
    int16_t loaded_rounds;
};

struct WeaponAmmoPickupMidReload {
    int32_t weapon_id;
    int16_t magazine_id;
    int16_t rounds_picked_up;
};

struct WeaponFinishReload : public WeaponStartReload {
};

struct WeaponCancelReload : public WeaponStartReload {
};

struct NetgameEquipmentNew {
    int32_t object_id;
    MemRef tag_id;
    MemRef netgam_equipment_tag_id;
};

struct ProjectileDetonate {
    int32_t object_id;
    Vec3d position;
};

struct ItemAccelerate {
    int32_t object_id;
    float magnitude;
    Vec3d direction;
};

struct DamageDealt {
    int32_t damaged_object_id;
    float shield_damage;
    bool shields_depleted;
};

struct ProjectileAttach {
    int32_t projectile_object_id;
    int32_t parent_object_id;
    int16_t parent_node_id;
};

struct ClientToServerPong {
    int8_t player_id;
};

struct SuperPingUpdate {
    int8_t player_id;
    PAD(3);
    int32_t player_ping;
};

struct SvMotd {
    char text[256]; // Confirm
};

struct RconRequest {
    char password[9];
    char command[64];
};

struct RconResponse {
    char text[81];
};

struct VulpesMessage {
    uint16_t payload_size;
    union {
        uint8_t  payload8[1000];
        uint16_t payload16[1000/2];
        uint32_t payload32[1000/4];
    };
}; static_assert(sizeof(VulpesMessage) == 1002);

#pragma pack(pop)
