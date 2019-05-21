#pragma once
#include "../types.hpp"

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

Table** effect_ptrs();
