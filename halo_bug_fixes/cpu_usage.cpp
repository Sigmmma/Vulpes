#include "cpu_usage.hpp"
#include "../hooks/hooker.hpp"

#include <Windows.h>
#include <cstdint>

uintptr_t sleep = reinterpret_cast<uintptr_t>(&Sleep);

__attribute__((naked))
void replacement_function(){
    asm (
        "push 1;\n"
        "call %0;\n"
        "mov al, BYTE PTR ss:[esp+0x1b];\n"
        "ret 4;\n"
        : "+m" (sleep)
    );
}

// Used for fix 1 and 2
static Signature(false, sig_cpu_usage_fix_pattern1, {0xFF, 0xD6, 0x8A, 0x44, 0x24, 0x13, 0x84, 0xC0, 0x74});
const int PATTERN1_PATCH_LEN = 6;
// Used for fix 3, 4, 5
static Signature(false, sig_cpu_usage_fix_pattern2, {0xFF, 0x15, -1, -1, -1, -1, 0x8A, 0x44, 0x24, 0x13, 0x84, 0xC0, 0x74});
const int PATTERN2_PATCH_LEN = 10;

static Patch(cpu_usage_fix_patch1);
static Patch(cpu_usage_fix_patch2);
static Patch(cpu_usage_fix_patch3);
static Patch(cpu_usage_fix_patch4);
static Patch(cpu_usage_fix_patch5);

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
        if (cpu_usage_fix_patch1.is_built()) {cpu_usage_fix_patch1.apply();};
        if (cpu_usage_fix_patch2.is_built()) {cpu_usage_fix_patch2.apply();};
        if (cpu_usage_fix_patch3.is_built()) {cpu_usage_fix_patch3.apply();};
        if (cpu_usage_fix_patch4.is_built()) {cpu_usage_fix_patch4.apply();};
        if (cpu_usage_fix_patch5.is_built()) {cpu_usage_fix_patch5.apply();};
    };
    applied = true;
}

void revert_cpu_usage_fixes(){
    if (cpu_usage_fix_patch1.is_applied()) {cpu_usage_fix_patch1.revert();};
    if (cpu_usage_fix_patch2.is_applied()) {cpu_usage_fix_patch2.revert();};
    if (cpu_usage_fix_patch3.is_applied()) {cpu_usage_fix_patch3.revert();};
    if (cpu_usage_fix_patch4.is_applied()) {cpu_usage_fix_patch4.revert();};
    if (cpu_usage_fix_patch5.is_applied()) {cpu_usage_fix_patch5.revert();};
    applied = false;
}
