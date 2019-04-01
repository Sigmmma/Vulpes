
static MemRef* object_full_ids[2048];

enum ObjectTypes : int8_t{
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

class ObjectHeader {
public:
    uint8_t active:1;
    uint8_t visible:1;
    uint8_t newly_created:1;
    uint8_t flagged_for_deletion:1;
    uint8_t is_child:1;
    uint8_t is_connected_to_map:1;
    uint8_t automatic_deactivation_enabled:1;
    uint8_t unknown7:1;
    ObjectTypes type;
    int16_t cluster_id;

    int16_t data_size;
    // int16_t unused

    void* object_data;

    void delete(){
        flagged_for_deletion = 1;
        // TODO go through each child and set them up for deletion too.
    }
};static_assert(sizeof(ObjectHeader) == 0xC);


enum ObjectNetworkRole : uint32_t{
    MASTER,
    PUPPET,
    LOCALLY_CONTROLLED_PUPPET,
    LOCAL_ONLY
};

enum ObjectAttachmentType : int8_t{
    INVALID = -1,
    LIGHT = 0,
    LOOPING_SOUND,
    EFFECT,
    CONTRAIL,
    PARTICLE
};
