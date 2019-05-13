#include "table.hpp"

#include "../hooks/hooker.hpp"

Signature(true, sig_object_table,
    {-1, -1, -1, -1, 0x8B, 0x51, 0x34, 0x25, 0xFF, 0xFF, 0x00, 0x00, 0x8D, 0x04, 0x40, 0x8B, 0x44, 0x82, 0x08, 0x8B, 0x08});

Signature(true, sig_weather_particle_table,
    {-1, -1, -1, -1, 0x89, 0x2D, -1, -1, -1, -1, 0x66, 0x89, 0x3D, -1, -1, -1, -1, 0x88, 0x5E, 0x24});

uintptr_t* effect_ptrs_lst;

uintptr_t* effect_ptrs(){
    static uintptr_t sig_addr1 = sig_weather_particle_table.get_address();
    if (!effect_ptrs_lst){
        effect_ptrs_lst = (uintptr_t*)sig_addr1;
    };
    return effect_ptrs_lst;
}

Table* weather_particles_table(){
    static uintptr_t* effect_ptrs_loc = effect_ptrs();
    return (Table*)effect_ptrs_loc[0];
}

Table* particle_table(){
    static uintptr_t* effect_ptrs_loc = effect_ptrs();
    return (Table*)effect_ptrs_loc[1];
}

Table* particle_system_particles_table(){
    static uintptr_t* effect_ptrs_loc = effect_ptrs();
    return (Table*)effect_ptrs_loc[2];
}

Table* effect_table(){
    static uintptr_t* effect_ptrs_loc = effect_ptrs();
    return (Table*)effect_ptrs_loc[3];
}

Table* effect_locations_table(){
    static uintptr_t* effect_ptrs_loc = effect_ptrs();
    return (Table*)effect_ptrs_loc[4];
}

Table* decals_table(){
    static uintptr_t* effect_ptrs_loc = effect_ptrs();
    return (Table*)effect_ptrs_loc[5];
}

Table* contrail_points_table(){
    static uintptr_t* effect_ptrs_loc = effect_ptrs();
    return (Table*)effect_ptrs_loc[6];
}

Table* contrail_table(){
    static uintptr_t* effect_ptrs_loc = effect_ptrs();
    return (Table*)effect_ptrs_loc[7];
}

Table* device_groups_table(){
    static uintptr_t* effect_ptrs_loc = effect_ptrs();
    return (Table*)effect_ptrs_loc[8];
}

ObjectTable* object_table(){
    static uintptr_t* object_table_ptr = (uintptr_t*)sig_object_table.get_address();
    return (ObjectTable*)object_table_ptr[0];
}
