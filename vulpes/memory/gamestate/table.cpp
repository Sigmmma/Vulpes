/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <hooker/hooker.hpp>

#include <vulpes/functions/table.hpp>

#include "table.hpp"

MemRef Table::allocate(){
    MemRef return_value;
    return_value.raw = datum_new(this);
    return return_value;
}
/*
MemRef Table::new_at(MemRef id){
    MemRef return_value;
    return_value.raw = datum_new_at_index(this, id.raw);
    return return_value;
}
*/
bool Table::remove(MemRef id){
    return (datum_delete(this, id.raw) != 0xFFFFFFFF);
}

// Important, used for all effect related stuff
Signature(true, sig_weather_particle_table,
    {-1, -1, -1, -1, 0x89, 0x2D, -1, -1, -1, -1, 0x66, 0x89, 0x3D,
     -1, -1, -1, -1, 0x88, 0x5E, 0x24});

GenericTable** effect_ptrs_lst;

GenericTable** effect_ptrs(){
    static uintptr_t sig_addr1 = sig_weather_particle_table.address();
    if (!effect_ptrs_lst){
        effect_ptrs_lst = reinterpret_cast<GenericTable**>(sig_addr1);
    }
    return effect_ptrs_lst;
}
