/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <hooker/hooker.hpp>

#include <vulpes/memory/signatures.hpp>

#include "cpu_usage.hpp"

extern "C" {
    extern cpu_usage_sleep_replacement();
}

// Used for fix 1 and 2
Signature(false, sig_cpu_usage_fix_pattern1,
    {0xFF, 0xD6, 0x8A, 0x44, 0x24, 0x13, 0x84, 0xC0, 0x74});
// Used for fix 3, 4, 5
Signature(false, sig_cpu_usage_fix_pattern2,
    {0xFF, 0x15, -1, -1, -1, -1, 0x8A, 0x44, 0x24, 0x13, 0x84, 0xC0, 0x74});

Patch(cpu_usage_fix_patch1, 0,  6, CALL_PATCH, &cpu_usage_sleep_replacement);
Patch(cpu_usage_fix_patch2, 0,  6, CALL_PATCH, &cpu_usage_sleep_replacement);
Patch(cpu_usage_fix_patch3, 0, 10, CALL_PATCH, &cpu_usage_sleep_replacement);
Patch(cpu_usage_fix_patch4, 0, 10, CALL_PATCH, &cpu_usage_sleep_replacement);
Patch(cpu_usage_fix_patch5, 0, 10, CALL_PATCH, &cpu_usage_sleep_replacement);

void init_cpu_usage_fixes() {

    auto pat1 = fix_cpu_usage_pattern1();
    auto pat2 = fix_cpu_usage_pattern2();
    if (pat1->size() >= 1) {
        if (cpu_usage_fix_patch1.build(pat1->at(0))) cpu_usage_fix_patch1.apply();
    }
    if (pat1->size() >= 2) {
        if (cpu_usage_fix_patch2.build(pat1->at(1))) cpu_usage_fix_patch2.apply();
    }
    if (pat2->size() >= 1) {
        if (cpu_usage_fix_patch3.build(pat2->at(0))) cpu_usage_fix_patch3.apply();
    }
    if (pat2->size() >= 2) {
        if (cpu_usage_fix_patch4.build(pat2->at(1))) cpu_usage_fix_patch3.apply();
    }
    if (pat2->size() >= 3) {
        if (cpu_usage_fix_patch5.build(pat2->at(2))) cpu_usage_fix_patch3.apply();
    }
}

void revert_cpu_usage_fixes() {
    cpu_usage_fix_patch1.revert();
    cpu_usage_fix_patch2.revert();
    cpu_usage_fix_patch3.revert();
    cpu_usage_fix_patch4.revert();
    cpu_usage_fix_patch5.revert();
}
