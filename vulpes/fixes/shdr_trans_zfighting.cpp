/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
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
Patch(patch_shader_trans_zfighting2_fix, 0, 9, SKIP_PATCH, 0);
// These patches forces the decal bias pointers to be explicitly overwritten
// so weird config.txt setups can't break this fix.
Patch(patch_shader_trans_zfighting3a_fix, 0, 4, INT_PATCH, &val_DecalZBiasValue);
Patch(patch_shader_trans_zfighting3b_fix, 0, 4, INT_PATCH, &val_DecalSlopeZBiasValue);
Patch(patch_shader_trans_zfighting3c_fix, 0, 4, INT_PATCH, &val_DecalZBiasValue);
Patch(patch_shader_trans_zfighting3d_fix, 0, 4, INT_PATCH, &val_DecalSlopeZBiasValue);

void init_shdr_trans_zfighting_fixes() {
    if (patch_shader_trans_zfighting2_fix.build(
            fix_shader_trans_zfighting2())) {
        patch_shader_trans_zfighting2_fix.apply();
    }
    auto sig_addr3 = fix_shader_trans_zfighting3();
    if (sig_addr3) {
        if (patch_shader_trans_zfighting3a_fix.build(sig_addr3 +  0xE)
         && patch_shader_trans_zfighting3b_fix.build(sig_addr3 + 0x34)
         && patch_shader_trans_zfighting3c_fix.build(sig_addr3 +  0xE + 0x50)
         && patch_shader_trans_zfighting3d_fix.build(sig_addr3 + 0x34 + 0x50)) {
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
