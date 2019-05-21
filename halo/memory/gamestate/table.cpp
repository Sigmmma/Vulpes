#include "table.hpp"
#include "../../../hooker/hooker.hpp"


// Important, used for all effect related stuff
Signature(true, sig_weather_particle_table,
    {-1, -1, -1, -1, 0x89, 0x2D, -1, -1, -1, -1, 0x66, 0x89, 0x3D, -1, -1, -1, -1, 0x88, 0x5E, 0x24});

Table** effect_ptrs_lst;

Table** effect_ptrs(){
    static uintptr_t sig_addr1 = sig_weather_particle_table.address();
    if (!effect_ptrs_lst){
        effect_ptrs_lst = reinterpret_cast<Table**>(sig_addr1);
    };
    return effect_ptrs_lst;
}
