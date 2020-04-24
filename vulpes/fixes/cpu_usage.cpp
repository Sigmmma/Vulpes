/*
 * This file is part of Vulpes, an extension of Halo Custom Edition's capabilities.
 * Copyright (C) 2019-2020 gbMichelle (Michelle van der Graaf)
 *
 * Vulpes is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, version 3.
 *
 * Vulpes is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * long with Vulpes.  If not, see <https://www.gnu.org/licenses/agpl-3.0.en.html>
 */

#include <hooker/hooker.hpp>

#include <vulpes/memory/signatures/signatures.hpp>

#include "cpu_usage.hpp"

extern "C" {
    extern cpu_usage_sleep_replacement();
}

static Patch(cpu_usage_fix_patch1, NULL,  6,
    CALL_PATCH, &cpu_usage_sleep_replacement);
static Patch(cpu_usage_fix_patch2, NULL,  6,
    CALL_PATCH, &cpu_usage_sleep_replacement);
static Patch(cpu_usage_fix_patch3, NULL, 10,
    CALL_PATCH, &cpu_usage_sleep_replacement);
static Patch(cpu_usage_fix_patch4, NULL, 10,
    CALL_PATCH, &cpu_usage_sleep_replacement);
static Patch(cpu_usage_fix_patch5, NULL, 10,
    CALL_PATCH, &cpu_usage_sleep_replacement);

void init_cpu_usage_fixes() {
    auto pat1 = sig_fix_cpu_usage_pattern1();
    auto pat2 = sig_fix_cpu_usage_pattern2();
    if (pat1.size() >= 1) {
        if (cpu_usage_fix_patch1.build(pat1[0])) cpu_usage_fix_patch1.apply();
    }
    if (pat1.size() >= 2) {
        if (cpu_usage_fix_patch2.build(pat1[1])) cpu_usage_fix_patch2.apply();
    }
    if (pat2.size() >= 1) {
        if (cpu_usage_fix_patch3.build(pat2[0])) cpu_usage_fix_patch3.apply();
    }
    if (pat2.size() >= 2) {
        if (cpu_usage_fix_patch4.build(pat2[1])) cpu_usage_fix_patch4.apply();
    }
    if (pat2.size() >= 3) {
        if (cpu_usage_fix_patch5.build(pat2[2])) cpu_usage_fix_patch5.apply();
    }
}

void revert_cpu_usage_fixes() {
    cpu_usage_fix_patch1.revert();
    cpu_usage_fix_patch2.revert();
    cpu_usage_fix_patch3.revert();
    cpu_usage_fix_patch4.revert();
    cpu_usage_fix_patch5.revert();
}
