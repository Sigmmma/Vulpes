/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <hooker/hooker.hpp>

#include "file_handle_leak.hpp"

const uint32_t NEW_ALLOCATION_SIZE = 0x24000000;

Signature(false, sig_file_handle_leak1,
    {0x05, 0x00, 0x00, 0x00, 0x08, 0x3B, 0x44});
Signature(false, sig_file_handle_leak2,
    {0x81, 0xC1, 0x00, 0x00, 0x00, 0x08});
Signature(false, sig_file_handle_leak3,
    {0x05, 0x00, 0x00, 0x00, 0x08, 0x3B, 0xC8});

Patch(file_handle_leak_patch1, sig_file_handle_leak1, 1, 4, INT_PATCH, NEW_ALLOCATION_SIZE);
Patch(file_handle_leak_patch2, sig_file_handle_leak2, 2, 4, INT_PATCH, NEW_ALLOCATION_SIZE);
Patch(file_handle_leak_patch3, sig_file_handle_leak3, 1, 4, INT_PATCH, NEW_ALLOCATION_SIZE);

void init_file_handle_leak_fixes(){
    if (file_handle_leak_patch1.build()
    &&  file_handle_leak_patch2.build()
    &&  file_handle_leak_patch3.build()){
        file_handle_leak_patch1.apply();
        file_handle_leak_patch2.apply();
        file_handle_leak_patch3.apply();
    }
}

void revert_file_handle_leak_fixes(){
    file_handle_leak_patch1.revert();
    file_handle_leak_patch2.revert();
    file_handle_leak_patch3.revert();
}
