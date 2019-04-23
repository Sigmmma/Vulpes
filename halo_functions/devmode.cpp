#include "../hooks/hooker.hpp"
#include <cstdint>

Signature(false, sig_developer_mode_address,
    {-1, -1, -1, -1, 0x81, 0xEC, 0x1C, 0x02, 0x00, 0x00, 0x3C, 0x03, 0x0F, 0x82, -1, -1, -1, -1, 0xA0});

uint8_t developer_mode_level(){
    static uint8_t* developer_mode = (uint8_t*)*(uintptr_t*)sig_developer_mode_address.get_address();
    if (developer_mode){
        return *developer_mode;
    }else{
        return 0;
    };
}
