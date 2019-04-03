#include "../memory/player.hpp"

// Probably not going to use these, but I spent time enumerating them.
enum MessageType {
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
    RCON_RESPONSE                      = 0x38
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
};

struct ObjectUpdateHeaderTimestamped : public ObjectUpdateHeader {
    uint8_t pad;
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
    uint8_t pad[3];
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
    uint8_t pad[3];
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
    int16_t pad;
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




#pragma pack(pop)



enum HudChatType : int8_t {
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
