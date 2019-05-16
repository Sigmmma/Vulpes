#include "host_refusal.hpp"
#include "../../hooker/hooker.hpp"

Signature(false, sig_host_refusal1, {0x88, 0x5C, 0x24, 0x2A, 0xE8, -1, -1, -1, -1, 0x50, 0xE8});
const size_t HOST_REFUSAL_CLIENT1_LEN = 5;
const uint32_t HOST_REFUSAL_CLIENT1_OFFSET = 0xA;

Signature(false, sig_host_refusal2, {0x8B, 0x7F, 0x08, 0x3B, 0xFD, 0x74, 0x5F, 0x8D, 0x9B, 0x00, 0x00, 0x00, 0x00});
const uint32_t HOST_REFUSAL_CLIENT2_OFFSET = 5;

Signature(false, sig_client_refusal, {-1, -1, 0x75, 0x11, 0x6A, 0x01, 0x6A, 0x7E, 0x68, 0xA0, 0x00, 0x00, 0x00});

Patch(host_refusal_fix1);
Patch(host_refusal_fix2);
Patch(client_refusal_fix);

void init_host_refusal_fixes(){
    if (sig_host_refusal1.get_address() && sig_host_refusal2.get_address()){
        host_refusal_fix1.build_old(
            sig_host_refusal1.get_address()+HOST_REFUSAL_CLIENT1_OFFSET,
            HOST_REFUSAL_CLIENT1_LEN, NOP_PATCH, 0);
        host_refusal_fix2.build_manual(
            sig_host_refusal2.get_address()+HOST_REFUSAL_CLIENT2_OFFSET, {0xEB});
    };
    if (sig_client_refusal.get_address()){
        client_refusal_fix.build_manual(
            sig_client_refusal.get_address(), {0xEB, 0x13});
    };

    if (host_refusal_fix1.is_built() && host_refusal_fix2.is_built()){
        host_refusal_fix1.apply();
        host_refusal_fix2.apply();
    };
    if (client_refusal_fix.is_built()){
        client_refusal_fix.apply();
    };
}

void revert_host_refusal_fixes(){
    host_refusal_fix1.revert();
    host_refusal_fix2.revert();
    client_refusal_fix.revert();
}
