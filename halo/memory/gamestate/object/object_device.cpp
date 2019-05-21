#include "object_device.hpp"

Table* device_groups_table(){
    static Table** effect_ptrs_loc = effect_ptrs();
    return effect_ptrs_loc[8];
}
