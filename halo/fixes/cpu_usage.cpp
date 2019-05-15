#include "cpu_usage.hpp"
#include "../../hooker/hooker.hpp"
#define WIN32_MEAN_AND_LEAN
#include <windows.h>

uintptr_t sleep = reinterpret_cast<uintptr_t>(&Sleep);

__attribute__((naked))
void replacement_function(){
    asm (
        "push 1;"
        "call %0;"
        "mov al, BYTE PTR ss:[esp+0x1b];"
        "ret 4;"
        : "+m" (sleep)
    );
}

// Used for fix 1 and 2
Signature(false, sig_cpu_usage_fix_pattern1, {0xFF, 0xD6, 0x8A, 0x44, 0x24, 0x13, 0x84, 0xC0, 0x74});
const int PATTERN1_PATCH_LEN = 6;
// Used for fix 3, 4, 5
Signature(false, sig_cpu_usage_fix_pattern2, {0xFF, 0x15, -1, -1, -1, -1, 0x8A, 0x44, 0x24, 0x13, 0x84, 0xC0, 0x74});
const int PATTERN2_PATCH_LEN = 10;

Patch(cpu_usage_fix_patch1);
Patch(cpu_usage_fix_patch2);
Patch(cpu_usage_fix_patch3);
Patch(cpu_usage_fix_patch4);
Patch(cpu_usage_fix_patch5);

static bool applied = false;

void init_cpu_usage_fixes(){
    static bool already_initialized = false;
    if (!already_initialized){
        uintptr_t addr1 = sig_cpu_usage_fix_pattern1.get_address(true);
        if (addr1){
            cpu_usage_fix_patch1.build(addr1,
                PATTERN1_PATCH_LEN, CALL_PATCH, reinterpret_cast<uintptr_t>(&replacement_function));
            cpu_usage_fix_patch1.apply();
        };
        uintptr_t addr2 = sig_cpu_usage_fix_pattern1.get_address(true);
        if (addr2){
            cpu_usage_fix_patch2.build(addr2,
                PATTERN1_PATCH_LEN, CALL_PATCH, reinterpret_cast<uintptr_t>(&replacement_function));
            cpu_usage_fix_patch2.apply();
        };
        uintptr_t addr3 = sig_cpu_usage_fix_pattern2.get_address(true);
        if (addr3){
            cpu_usage_fix_patch3.build(addr3,
                PATTERN2_PATCH_LEN, CALL_PATCH, reinterpret_cast<uintptr_t>(&replacement_function));
            cpu_usage_fix_patch3.apply();
        };
        uintptr_t addr4 = sig_cpu_usage_fix_pattern2.get_address(true);
        if (addr4){
            cpu_usage_fix_patch4.build(addr4,
                PATTERN2_PATCH_LEN, CALL_PATCH, reinterpret_cast<uintptr_t>(&replacement_function));
            cpu_usage_fix_patch4.apply();
        };
        uintptr_t addr5 = sig_cpu_usage_fix_pattern2.get_address(true);
        if (addr5){
            cpu_usage_fix_patch5.build(addr5,
                PATTERN2_PATCH_LEN, CALL_PATCH, reinterpret_cast<uintptr_t>(&replacement_function));
            cpu_usage_fix_patch5.apply();
        };
        already_initialized = true;
    }else{
        if (cpu_usage_fix_patch1.is_built()) cpu_usage_fix_patch1.apply();
        if (cpu_usage_fix_patch2.is_built()) cpu_usage_fix_patch2.apply();
        if (cpu_usage_fix_patch3.is_built()) cpu_usage_fix_patch3.apply();
        if (cpu_usage_fix_patch4.is_built()) cpu_usage_fix_patch4.apply();
        if (cpu_usage_fix_patch5.is_built()) cpu_usage_fix_patch5.apply();
    };
    applied = true;
}

void revert_cpu_usage_fixes(){
    cpu_usage_fix_patch1.revert();
    cpu_usage_fix_patch2.revert();
    cpu_usage_fix_patch3.revert();
    cpu_usage_fix_patch4.revert();
    cpu_usage_fix_patch5.revert();
    applied = false;
}
