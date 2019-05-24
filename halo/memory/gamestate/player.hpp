/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#pragma once
#include "table.hpp"
#include "object/object.hpp"

enum class InteractionType : uint16_t {
    NONE                    = 0,
    PICKUP_POWERUP          = 1,
    SWAP_POWERUP            = 2,
    EXIT_SEAT               = 3,
    FOUR                    = 4, // Unknown

    SWAP_EQUIPMENT          = 5,
    SWAP_WEAPON             = 6,
    PICKUP_WEAPON           = 7,

    ENTER_SEAT              = 8,
    FORCE_AI_TO_EXIT_SEAT   = 9,
    TOUCH_DEVICE            = 10,
    FLIP_VEHICLE            = 11
};

enum class NetworkColor : uint16_t {
    WHITE   = 0,
    BLACK   = 1,
    RED     = 2,
    BLUE    = 3,
    GRAY    = 4,
    YELLOW  = 5,
    GREEN   = 6,
    PINK    = 7,
    PURPLE  = 8,
    CYAN    = 9,
    COBALT  = 10,
    ORANGE  = 11,
    TEAL    = 12,
    SAGE    = 13,
    BROWN   = 14,
    TAN     = 15,
    MAROON  = 16,
    SALMON  = 17
};

enum class PlayerObjectiveMode : int32_t {
    RACE = 22,
    HILL = 34,
    JUGGERNAUT = 35,
    BALL = 41,
    NONE = -1
};

union PlayerMultiplayerStatistics {
    uint32_t _pad[2];

    struct Ctf {
        int16_t flag_grabs;
        int16_t flag_returns;
        int16_t flag_scores;
    } ctf;

    struct Slayer {
    } slayer;

    struct Oddball {
        int16_t _unknown;
        int16_t target_kills;
        int16_t kills;
    } oddball;

    struct King {
        int16_t hill_score;
    } king;

    struct Race {
        int16_t time;
        int16_t laps;
        int16_t best_time;
    } race;
}; static_assert(sizeof(PlayerMultiplayerStatistics) == 8);
/*
typedef struct {
    MemRef player_id;
    int8_t update_id;
    int8_t baseline_id;
} s_remote_player_action_update_header;
static_assert(sizeof(s_remote_player_action_update_header) == 6);



typedef struct {
    union {
        struct {
            int32_t last_acknowledged_sequence_number;      // E8
            int32_t last_acknowledged_update_id;            // EC
            Vec3d   update_data; // Position update data    // F0
        } local;
        struct {
            int32_t last_acknowledged_update_id;            // E8
            int32_t last_acknowledged_baseline_id;          // EC
            PlayerUpdateNetworkData_RemoteActionUpdate update_data; // 0xF0
        } remote;
    };
    PlayerActionQueue action_queue;
    struct {
        int32_t last_acknowledged_sequence_number;
        int32_t last_acknowledged_update_id;
        PlayerUpdateNetworkData_RemotePositionUpdate
    }
} PlayerNetworkUpdateData;
static_assert(sizeof(PlayerNetworkUpdateData) == 0x114);*/
#pragma pack(push, 1)

class Player {
public:
    int16_t      _id;                       // 0
    int16_t      host;                      // 2
    int16_t      name[12];                  // 4
    MemRef        _unknown;                 // 1C
    Team         team;                      // 20
    PAD(3);                                 // color?
    MemRef        interaction_obj;          // 24
    InteractionType interaction_obj_type;   // 28
    int16_t      interaction_obj_seat;      // 2A
    int32_t      respawn_time;              // 2C
    int32_t      respawn_time_growth;       // 30
    MemRef       obj_id;                    // 34
    MemRef       prev_obj_id;               // 38
    int16_t      bsp_cluster_id;            // 3C
    bool         weapon_swap_result;        // 3E
    PAD(1);
    MemRef        auto_aim_target_obj;      // 40
    int32_t      last_fired_tick;           // 44
    int16_t      network_name[12];          // 48
    NetworkColor color;                     // 60
    int16_t      icon_index;                // 62
    int8_t       machine_id;                // 64
    int8_t       controller_id;             // 65
    Team         team2;                     // 66
    int8_t       id;                        // 67
    // End of client stuct? - it was a comment on one of my reference materials.
    int16_t      camo_time_left;            // 68
    int16_t      other_powerup_time_left;   // 6A
    float        speed_multiplier;          // 6C
    int32_t      src_teleporter_netgame_flag_id; // 70

    PlayerObjectiveMode objective_mode;      // 74

    MemRef       objective_player;           // 78
    // Used for rendering the name of the player you are targeting.
    MemRef       target_player;              // 7C
    int32_t      target_player_time;         // 80
    int32_t      last_death_tick;            // 84
    MemRef       slayer_target_player;       // 88
    bool         odd_man_out;                // 8C
    PAD(3);
    PAD(4);                                  // 90
    PAD(2);
    int16_t      killstreak;                 // 96
    int16_t      multikill;                  // 98
    int16_t      last_kill_tick;             // 9A
    int16_t      kills[4];                   // 9C
    int16_t      assists[4];                 // A4
    int16_t      betrayals;                  // AC
    int16_t      deaths;                     // AE
    int16_t      suicides;                   // B0
    PAD(2);
    PAD(3*4);
    int16_t      team_kills;                 // C0
    PAD(2);

    PlayerMultiplayerStatistics mp_stats;    // C4

    int32_t      telefrag_ticks;             // CC
    int32_t      quit_at_tick;               // D0
    bool         telefrag_danger;            // D4
    bool         quit;                       // D5
    PAD(2);
    PAD(4);                                  // D8
    int32_t      ping;                       // DC
    int32_t      team_kill_count;            // E0
    int32_t      team_kill_ticks_since_last; // E4

    uint8_t      _network_data[ 0x200-0xE8 ]; // E8 - 200

}; static_assert(sizeof(Player) == 0x200);

#pragma pack(pop)
