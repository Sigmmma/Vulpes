#pragma once
#include "types.hpp"

struct ScenarioLocation {
    int32_t leaf_id;
    int16_t cluster_id;
    PAD(2);
}; static_assert( sizeof(ScenarioLocation) == 0x8 );

enum class Team : int8_t {
    RED         = 0,
    BLUE        = 1,

    NONE        = 0,
    PLAYER      = 1,
    HUMAN       = 2,
    COVENANT    = 3,
    FLOOD       = 4,
    SENTINEL    = 5,
    UNUSED_6    = 6,
    UNUSED_7    = 7
};

enum class ObjectType : int8_t{
    OBJECT = -4,
    DEVICE = -3,
    ITEM = -2,
    UNIT = -1,
    BIPED = 0,
    VEHICLE,
    WEAPON,
    EQUIPMENT,
    GARBAGE,
    PROJECTILE,
    SCENERY,
    MACHINE,
    CONTROL,
    LIGHT_FIXTURE,
    PLACEHOLDER,
    SOUND_SCENERY
};

enum class ObjectNetworkRole : uint32_t{
    MASTER = 0,
    PUPPET,
    LOCALLY_CONTROLLED_PUPPET,
    LOCAL_ONLY
};

enum class ObjectAttachmentType : int8_t{
    INVALID = -1,
    LIGHT = 0,
    LOOPING_SOUND,
    EFFECT,
    CONTRAIL,
    PARTICLE
};

enum ObjectSizes {
    SIZE_OBJECT          = 0x1F4,
    SIZE_SCENERY         =     4 + SIZE_OBJECT,
    SIZE_PLACEHOLDER     =     8 + SIZE_OBJECT,
    SIZE_SOUND_SCENERY   =     4 + SIZE_OBJECT,

    SIZE_DEVICE          =  0x20 + SIZE_OBJECT,
    SIZE_MACHINE         =  0x14 + SIZE_DEVICE,
    SIZE_CONTROL         =     8 + SIZE_DEVICE,
    SIZE_LIGHT_FIXTURE   =  0x18 + SIZE_DEVICE,

    SIZE_ITEM            =  0x38 + SIZE_OBJECT,
    SIZE_WEAPON          = 0x114 + SIZE_ITEM,
    SIZE_EQUIPMENT       =  0x68 + SIZE_ITEM,
    SIZE_GARBAGE         =  0x18 + SIZE_ITEM,
    SIZE_PROJECTILE      =  0x84 + SIZE_ITEM,

    SIZE_UNIT            = 0x2D8 + SIZE_OBJECT,
    SIZE_BIPED           =  0x84 + SIZE_UNIT,
    SIZE_VEHICLE         =  0xF4 + SIZE_UNIT
};

#pragma pack(push, 1)

struct ObjectPlacementData {
    MemRef tag;
    uint32_t _flags;
    MemRef player;
    MemRef owner_object;
    int32_t _unknown;
    ObjectType type;
    PAD(1);
    int16_t variant_id;
    Vec3d position;
    float angle; //maybe?
    Vec3d transitional_velocity;
    Vec3d forward;
    Vec3d up;
    Vec3d angular_velocity;
    RGBFloat change_colors[4];
}; static_assert(sizeof(ObjectPlacementData) == 0x88);

class Object {
public:
    MemRef tag;
    // Multiplayer related stuff
    struct ObjectNetwork {
        ObjectNetworkRole role; // 4
        bool _unknown;                  // 8
        bool should_force_baseline_update;  // 9
        PAD(2);
        int32_t existence_ticks; // C
    } base_network;
    bool hidden : 1;// 10
    bool on_ground : 1;
    bool ignore_gravity : 1;
    bool in_water : 1;
    bool _unknown_bit4 : 1;
    bool at_rest : 1;
    bool _unknown_bit6 : 1;
    bool no_collision2 : 1; // Confirm this
    bool _unknown_bit8 : 1;
    bool _unknown_bit9 : 1;
    bool has_sound_looping_attachment : 1; // Confirm this
    bool connected_to_map : 1;
    bool not_placed_automatically : 1;
    bool is_device_machine : 1;
    bool is_elevator : 1;
    bool is_elevator2 : 1;
    bool not_garbage : 1; // Prevents it from being cleaned up in garbage collection.
    bool _unknown_bit17 : 1;
    bool does_not_cast_shadow : 1;
    bool delete_at_deactivation : 1;
    bool do_not_reactivate : 1;
    bool out_of_bounds : 1;
    bool beautify : 1;
    bool limping : 1;
    bool collidable : 1;
    bool has_collision_model : 1;
    bool _unknown_bit26 : 1; // message delta related. see object_type_should_force_baseline_update
    bool _unknown_bit27 : 1; // message delta related. see *_process_update_delta
    // These should be documented specifically so we can avoid using them.
    bool opensauce_is_transforming_in : 1;
    bool opensauce_is_transforming_out : 1;
    bool _pad_bits : 2;

    int32_t object_marker_id; // 14

    struct ObjectNetworkDelta {
        bool valid_position;
        PAD(3);
        Vec3d position;

        bool valid_forward_and_up;
        PAD(3);
        Euler3d forward;
        Euler3d up;

        bool valid_transitional_velocity;
        PAD(3);
        Vec3d transitional_velocity;

        bool valid_timestamp;
        PAD(3);
        int32_t timestamp;
    } base_network_delta; static_assert(sizeof(ObjectNetworkDelta) == 0x44);

    Vec3d position;
    Vec3d transitional_velocity;
    Vec3d forward;
    Vec3d up;
    Vec3d angular_velocity;
    ScenarioLocation scenario_location;
    Vec3d center; // Used for bounding sphere
    float radius; // Bounding sphere radius
    float scale;
    ObjectType type;
    PAD(3);
    Team owner_team; //
    PAD(1);
    int16_t name_list_id;
    // Ticks not spent at_rest, only biped updates this.
    int16_t moving_time;
    int16_t variant_id; // for region permuations.
    MemRef player;
    MemRef owner_object;
    MemRef self_ref; // Member added for Vulpes. Helps with calling Halo functions.
    struct ObjectAnimationData {
        MemRef tag;
        int16_t id;
        int16_t frame;
        int16_t interpolation_frame;
        int16_t interpolation_frame_count;
    } animation; static_assert(sizeof(ObjectAnimationData) == 0xC);
    struct ObjectVitalityData {
        float max_health;
        float max_shield;
        float health;
        float shield;
        float current_shield_damage;
        float current_health_damage;
        // when this object is damaged, the 'entangled' object will also get damaged.
        // this is an immediate link, the entangled object's parent chain or 'entangled' reference isn't walked
        MemRef entangled_object;
        float recent_shield_damage;
        float recent_health_damage;
        int32_t shield_damage_update_tick;
        int32_t health_damage_update_tick;
        int16_t shield_stun_ticks;
        bool health_damage_effect_applied : 1;
        bool shield_damage_effect_applied : 1;
        bool health_depleted : 1;
        bool shield_depleted : 1;
        bool shield_related : 1;
        bool killed : 1;
        bool killed_silent : 1;
        bool _cannot_melee_attack : 1; //confirm if true
        bool _unknown : 3;
        bool cannot_take_damage : 1;
        bool shield_recharging : 1;
        bool killed_no_stats : 1;
        bool _pad : 2;
    } vitals; static_assert(sizeof(ObjectVitalityData) == 0x30);
    PAD(4);
    MemRef cluster_partition;
    MemRef _unknown_object_related;
    MemRef next_object;
    MemRef first_object;
    MemRef parent_object;
    int8_t parent_attachment_node;
    int8_t _unknown2;
    bool force_shield_update;
    struct ObjectValidOutGoingFunctions {
        // confirm this
        bool a : 1;
        bool b : 1;
        bool c : 1;
        bool d : 1;
    } valid_outgoing_functions; static_assert(sizeof(ObjectValidOutGoingFunctions) == 1);
    float incoming_function_values[4];
    float outgoing_function_values[4];

    struct ObjectAttachmentsData {
        ObjectAttachmentType types[8];
        MemRef attachments[8];
        MemRef first_widget;
    }attachment_data; static_assert(sizeof(ObjectAttachmentsData) == 0x2C);
    MemRef cached_render_state;
    bool region_0_destroyed : 1;
    bool region_1_destroyed : 1;
    bool region_2_destroyed : 1;
    bool region_3_destroyed : 1;
    bool region_4_destroyed : 1;
    bool region_5_destroyed : 1;
    bool region_6_destroyed : 1;
    bool region_7_destroyed : 1;
    PAD(1);
    int16_t shader_permutation;
    uint8_t region_health[8];
    int8_t region_permutation_id[8];
    RGBFloat change_colors_lower[4];
    RGBFloat change_colors_upper[4];
    struct ObjectHeaderBlockReference {
        uint16_t size;
        uint16_t offset;
    } node_orientations[2],
      node_matrices_block;
}; static_assert(sizeof(Object) == SIZE_OBJECT);

class ObjectScenery : public Object {
    PAD(4);
}; static_assert(sizeof(ObjectScenery) == SIZE_SCENERY);

class ObjectPlaceholder : public Object {
    PAD(8);
}; static_assert(sizeof(ObjectPlaceholder) == SIZE_PLACEHOLDER);

class ObjectSoundScenery : public Object {
    PAD(4);
}; static_assert(sizeof(ObjectSoundScenery) == SIZE_SOUND_SCENERY);



class ObjectHeader {
public:
    int16_t salt_id;
    bool active:1;
    bool visible:1;
    bool newly_created:1;
    bool flagged_for_deletion:1;
    bool is_child:1;
    bool is_connected_to_map:1;
    bool automatic_deactivation_enabled:1;
    bool _unknown7:1;
    ObjectType type;

    PAD(2);
    int16_t data_size;

    void* object_data;
}; static_assert(sizeof(ObjectHeader) == 12);

#pragma pack(pop)
