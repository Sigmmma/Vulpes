#include "object.hpp"
#include "../../../../hooker/hooker.hpp"

Signature(true, sig_object_table,
    {-1, -1, -1, -1, 0x8B, 0x51, 0x34, 0x25, 0xFF, 0xFF, 0x00, 0x00, 0x8D, 0x04, 0x40, 0x8B, 0x44, 0x82, 0x08, 0x8B, 0x08});

ObjectTable* object_table(){
    static ObjectTable** object_table_ptr =
        reinterpret_cast<ObjectTable**>(sig_object_table.address());
    return object_table_ptr[0];
}
