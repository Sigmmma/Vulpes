/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#pragma once
#include "../../types.hpp"

enum class GoalSignalling : uint32_t {
    MOTION_TRACKER = 0,
    NAVPOINT,
    DISABLED
};

enum class WeaponSet : uint32_t {
    NORMAL,
    PISTOLS,
    RIFLES,
    PLASMA,
    SNIPERS,
    NO_SNIPERS,
    ROCKETS,
    SHOTGUNS,
    SHORT_RANGE,
    HUMAN,
    COVENANT,
    CLASSIC,
    HEAVIES
};

enum class ItState : uint32_t {
    NONE,
    INVISIBLE,
    EXTRA_DAMAGE,
    DAMAGE_RESISTANT
};

enum class OddballCarrierSpeed : uint32_t {
    NORMAL,
    FASTER,
    SLOWER
};

enum class OddballBallType : uint32_t {
    NORMAL,
    REVERSE_TAG,
    JUGGERNAUT
};

enum class RaceType : uint32_t {
    NORMAL
    //Rally
    //that other one
};

#pragma pack(push, 1)

struct SharedVariantData {
    bool teams;
    PAD(3);
    bool show_players_on_radar : 1; // 4
    bool show_homies : 1;
    bool infinite_grenades : 1;
    bool players_have_shields : 1;
    bool invisible_players : 1;
    bool use_custom_start_equipment : 1;
    BITPAD(bool, 2);
    PAD(3);
    GoalSignalling goal_signalling; // 8
    bool odd_man_out; // C
    PAD(3);

    int32_t respawn_time_growth; // 10
    int32_t respawn_time;        // 14
    int32_t suicide_penalty;     // 18
    int32_t lives;               // 1C

    float health; //             // 20 I thought this was damage_multiplier...
    int32_t score_to_win;        // 24
    WeaponSet weapon_set;        // 28

    //////
    struct VehicleSet { // 2C // 30
        unsigned vehicle_set : 4;
        unsigned warthogs : 3;
        unsigned ghosts : 3;
        unsigned scorpions : 3;
        unsigned rwarthogs : 3;
        unsigned banshees : 3;
        unsigned cg_turrets : 3;
        unsigned custom1 : 3;
        unsigned custom2 : 3;
        unsigned custom3 : 3;
        unsigned pad : 1;
    } vehicle_sets[2]; static_assert(sizeof(VehicleSet) == 4);
    int32_t vehicles_respawn;     // 34

    int8_t friendly_fire;         // 38 // make an enum
    PAD(3);
    int32_t friendly_fire_penalty;// 3C
    bool team_autobalance;        // 40
    PAD(3);

    int32_t time_limit;           // 44
}; static_assert(sizeof(SharedVariantData) == 0x48);

struct CtfVariantData {
    bool assault;
    PAD(1);

    struct {
        bool reset;
        bool at_home_to_score;
        int32_t type; // 1 = single
    }flag;
}; static_assert(sizeof(CtfVariantData) == 0x8);

struct SlayerVariantData {
    bool death_handicap; // True: killed player's speed increases in magnitudes of one
    bool kill_handicap; // True: killer's speed decreases in magnitudes of two
    bool kill_in_order;
}; static_assert(sizeof(SlayerVariantData) == 0x3);

struct OddballVariantData {
    bool random_start;
    PAD(3);
    OddballCarrierSpeed ball_speed;

    ItState nonball_trait;
    ItState ball_trait;
    OddballBallType type;
    int32_t ball_count;
}; static_assert(sizeof(OddballVariantData) == 0x18);

struct KingVariantData {
    bool moving_hill;
}; static_assert(sizeof(KingVariantData) == 0x1);

struct RaceVariantData {
    RaceType race_type;
    uint32_t team_scoring;
}; static_assert(sizeof(RaceVariantData) == 0x8);

enum class GameVariant : uint32_t {
    NONE = 0,
    CTF,
    SLAYER,
    ODDBALL,
    KING,
    RACE,
    STUB
};

struct GameVariantData {
    wchar_t name[24];                           // 0x0
    GameVariant game_engine;                    // 0x30 Enums::game_engine
    SharedVariantData shared;                   // 0x34
    PAD(4*8);
    union {
        PAD(0x38); // This part of the struct should always take up this much space.
        CtfVariantData ctf;
        SlayerVariantData slayer;
        OddballVariantData oddball;
        KingVariantData king;
        RaceVariantData race;
    };
    uint32_t flags;                             // 0xD4
}; static_assert(sizeof(GameVariantData) == 0xD8);

#pragma pack(pop)

GameVariantData* game_variant_settings();
