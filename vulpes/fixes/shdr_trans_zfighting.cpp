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

#include <vulpes/memory/signatures.hpp>

#include "shdr_trans_zfighting.hpp"

// These are the values from config.txt
//float* DecalZBiasValue;
//float* DecalSlopeZBiasValue;
//float* TransparentDecalZBiasValue;
//float* TransparentDecalSlopeZBiasValue;

static const float val_DecalZBiasValue = -0.000055;
static const float val_DecalSlopeZBiasValue = -2.0;
static const float val_TransZBiasValue = -0.000005;
static const float val_TransSlopeZBiasValue = -1.0;

// All of these patches can work independently, for the sake of covering as
// many bases as possible I have implementesd them all.
// Patch 1 and patch 3 set the values. Which works for decals only.
// Patch 2 makes it so that non-decal transparents are also affected.

// This patch makes it so the default values for these config settings are never set.
//Patch(patch_shader_trans_zfighting1_fix);
// This patch makes it so that all shader transparent types are rendered with
// these bias values. Essentually as if all of them have the 'decal' flag set.
static Patch(patch_shader_trans_zfighting2_fix, NULL, 9,
    SKIP_PATCH, 0);
// These patches force the decal bias pointers to be explicitly overwritten
// so weird config.txt setups can't break this fix.
static Patch(patch_shader_trans_zfighting3a_fix, NULL, 4,
    INT_PATCH, &val_DecalZBiasValue);
static Patch(patch_shader_trans_zfighting3b_fix, NULL, 4,
    INT_PATCH, &val_DecalSlopeZBiasValue);
static Patch(patch_shader_trans_zfighting3c_fix, NULL, 4,
    INT_PATCH, &val_DecalZBiasValue);
static Patch(patch_shader_trans_zfighting3d_fix, NULL, 4,
    INT_PATCH, &val_DecalSlopeZBiasValue);

void init_shdr_trans_zfighting_fixes() {
    // TODO: Maybe just rewrite these parts of the game code with fully C++ code?
    if (patch_shader_trans_zfighting2_fix.build(
            sig_fix_shader_trans_zfighting2())) {
        patch_shader_trans_zfighting2_fix.apply();
    }
    auto z_offset_code = sig_fix_shader_trans_zfighting3();
    if (z_offset_code) {
        // All of these offsets are references to
        // DecalZBiasValue or DecalSlopeZBiasValue.
        if (patch_shader_trans_zfighting3a_fix.build(z_offset_code +  0xE)
         && patch_shader_trans_zfighting3b_fix.build(z_offset_code + 0x34)
         && patch_shader_trans_zfighting3c_fix.build(z_offset_code +  0xE + 0x50)
         && patch_shader_trans_zfighting3d_fix.build(z_offset_code + 0x34 + 0x50)) {
            patch_shader_trans_zfighting3a_fix.apply();
            patch_shader_trans_zfighting3b_fix.apply();
            patch_shader_trans_zfighting3c_fix.apply();
            patch_shader_trans_zfighting3d_fix.apply();
        }
    }
}

void revert_shdr_trans_zfighting_fixes() {
    patch_shader_trans_zfighting2_fix.revert();
    patch_shader_trans_zfighting3a_fix.revert();
    patch_shader_trans_zfighting3b_fix.revert();
    patch_shader_trans_zfighting3c_fix.revert();
    patch_shader_trans_zfighting3d_fix.revert();
}
