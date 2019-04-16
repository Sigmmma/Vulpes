#include "table.hpp"
#include <cstdio>

#include "../hooks/hooker.hpp"

Signature(true, sig_object_table,
    { 0x8B, 0x0D, -1, -1, -1, -1, 0x8B, 0x51, 0x34, 0x25, 0xFF, 0xFF, 0x00, 0x00, 0x8D });

static ObjectTable* object_table;

ObjectTable* get_object_table(){
    return object_table;
}

void init_tables(){

}
