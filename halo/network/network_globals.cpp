#include "network_globals.hpp"
#include "../../hooker/hooker.hpp"

Signature(true, sig_connection_type,
    {0x66, 0x83, 0x3D, -1, -1, -1, -1, 0x02, 0x75, -1, 0x8B});

ConnectionType get_connection_type(){
    static ConnectionType* connection_type_ptr = (ConnectionType*)*(uintptr_t*)(sig_connection_type.get_address() + 3);
    return *connection_type_ptr;
}
