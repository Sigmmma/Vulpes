#include "file_handle_leak.hpp"
#include "../../hooker/hooker.hpp"

const uint32_t NEW_ALLOCATION_SIZE = 0x24000000;

Signature(false, sig_file_handle_leak1, {0x05, 0x00, 0x00, 0x00, 0x08});
Signature(false, sig_file_handle_leak2, {0x81, 0xC1, 0x00, 0x00, 0x00, 0x08});
Signature(false, sig_file_handle_leak3, {0x05, 0x00, 0x00, 0x00, 0x08});

Patch(file_handle_leak_patch1);
Patch(file_handle_leak_patch2);
Patch(file_handle_leak_patch3);

static bool applied = false;

void init_file_handle_leak_fixes(){
    static bool already_initialized = false;
    if (!already_initialized){
        if (sig_file_handle_leak1.get_address()){
            file_handle_leak_patch1.build_int(sig_file_handle_leak1.get_address()+1, NEW_ALLOCATION_SIZE);
        };
        if (sig_file_handle_leak2.get_address()){
            file_handle_leak_patch2.build_int(sig_file_handle_leak2.get_address()+2, NEW_ALLOCATION_SIZE);
        };
        if (sig_file_handle_leak1.get_address() && sig_file_handle_leak3.get_address(sig_file_handle_leak1.get_address()+5)){
            file_handle_leak_patch3.build_int(sig_file_handle_leak3.get_address()+1, NEW_ALLOCATION_SIZE);
        };
    };
    if (file_handle_leak_patch1.is_built()
    &&  file_handle_leak_patch2.is_built()
    &&  file_handle_leak_patch3.is_built()){
        file_handle_leak_patch1.apply();
        file_handle_leak_patch2.apply();
        file_handle_leak_patch3.apply();
    };
    applied = true;
}

// I don't think it's worth it to be able to revert this patch.
// But, I want to keep this standardized.
void revert_file_handle_leak_fixes(){
    file_handle_leak_patch1.revert();
    file_handle_leak_patch2.revert();
    file_handle_leak_patch3.revert();
    applied = false;
}
