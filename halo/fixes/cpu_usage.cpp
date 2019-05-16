#include "cpu_usage.hpp"
#include "../../hooker/hooker.hpp"
#define WIN32_MEAN_AND_LEAN
#include <windows.h>

const auto sleep = &Sleep;

__attribute__((naked))
void replacement_function(){
    asm (
        "push 1;"
        "call %0;"
        "mov al, BYTE PTR ss:[esp+0x1b];"
        "ret 4;"
        :
        : "m" (sleep)
    );
}

// Used for fix 1 and 2
Signature(false, sig_cpu_usage_fix_pattern1,
    {0xFF, 0xD6, 0x8A, 0x44, 0x24, 0x13, 0x84, 0xC0, 0x74});
// Used for fix 3, 4, 5
Signature(false, sig_cpu_usage_fix_pattern2,
    {0xFF, 0x15, -1, -1, -1, -1, 0x8A, 0x44, 0x24, 0x13, 0x84, 0xC0, 0x74});

PatchNew(cpu_usage_fix_patch1, sig_cpu_usage_fix_pattern1, 0,  6, CALL_PATCH, &replacement_function);
PatchNew(cpu_usage_fix_patch2, sig_cpu_usage_fix_pattern1, 0,  6, CALL_PATCH, &replacement_function);
PatchNew(cpu_usage_fix_patch3, sig_cpu_usage_fix_pattern2, 0, 10, CALL_PATCH, &replacement_function);
PatchNew(cpu_usage_fix_patch4, sig_cpu_usage_fix_pattern2, 0, 10, CALL_PATCH, &replacement_function);
PatchNew(cpu_usage_fix_patch5, sig_cpu_usage_fix_pattern2, 0, 10, CALL_PATCH, &replacement_function);

void init_cpu_usage_fixes(){
    if (cpu_usage_fix_patch1.build()) cpu_usage_fix_patch1.apply();
    if (cpu_usage_fix_patch2.build()) cpu_usage_fix_patch2.apply();
    if (cpu_usage_fix_patch3.build()) cpu_usage_fix_patch3.apply();
    if (cpu_usage_fix_patch4.build()) cpu_usage_fix_patch4.apply();
    if (cpu_usage_fix_patch5.build()) cpu_usage_fix_patch5.apply();
}

void revert_cpu_usage_fixes(){
    cpu_usage_fix_patch1.revert();
    cpu_usage_fix_patch2.revert();
    cpu_usage_fix_patch3.revert();
    cpu_usage_fix_patch4.revert();
    cpu_usage_fix_patch5.revert();
}
