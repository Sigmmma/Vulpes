#pragma once
#include <cstdint>

#include "object.hpp"

class Table {
public:
    char name[0x20];
    int16_t max_count;
    int16_t index_size;
    int32_t one;
    int32_t data_v;
    int16_t zero;
    int16_t size;
    int16_t count;
    int16_t next_id;
};

class GenericTable : public Table {
public:
    void* first;
};

class ObjectTable : public Table {
public:
    ObjectHeader* objects;
};

ObjectTable* object_table();

Table* weather_particles_table();
Table* particle_table();
Table* particle_system_particles_table();
Table* effect_table();
Table* effect_locations_table();
Table* decals_table();
Table* contrail_points_table();
Table* contrail_table();
Table* device_groups_table();
