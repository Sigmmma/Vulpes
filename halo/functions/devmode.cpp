#include "../../hooker/hooker.hpp"

Signature(false, sig_developer_mode_address,
    {-1, -1, -1, -1, 0x04, 0x72, -1, 0x8D, 0x54, 0x24, 0x0C, 0x52});

uint8_t developer_mode_level(){
    static uint8_t* developer_mode = *(uint8_t**)sig_developer_mode_address.address();
    if (developer_mode){
        return *developer_mode;
    }else{
        return 0;
    };
}
