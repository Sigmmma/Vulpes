#include "shdr_trans_zfighting.hpp"
#include "../../hooker/hooker.hpp"

// These are the values from config.txt
//float* DecalZBiasValue;
//float* DecalSlopeZBiasValue;
//float* TransparentDecalZBiasValue;
//float* TransparentDecalSlopeZBiasValue;

const float val_DecalZBiasValue = -0.000055;
const float val_DecalSlopeZBiasValue = -2.0;
const float val_TransZBiasValue = -0.000005;
const float val_TransSlopeZBiasValue = -1.0;

Signature(false, sig_shader_trans_zfighting2,
    {0xE8, -1, -1, -1, -1, 0x84, 0xC0, 0x74, -1, 0xE8,
     -1, -1, -1, -1, 0x33, 0xC0, 0x89, 0x44, 0x24, 0x34, 0xEB});

Signature(false, sig_shader_trans_zfighting3,
    {0xF7, 0x05, -1, -1, -1, -1, 0x00, 0x00, 0x00, 0x04, 0x74,
     -1, 0x8B, 0x15, -1, -1, -1, -1, 0xA1, -1, -1, -1, -1, 0x8B, 0x08, 0x52});

// All of these patches can work independently, for the sake of covering as
// many bases as possible I have implementesd them all.
// Patch 1 and patch 3 set the values. Which works for decals only.
// Patch 2 makes it so that non-decal transparents are also affected.

// This patch makes it so the default values for these config settings are never set.
//Patch(patch_shader_trans_zfighting1_fix);
// This patch makes it so that all shader transparent types are rendered with
// these bias values. Essentually as if all of them have the 'decal' flag set.
Patch(patch_shader_trans_zfighting2_fix, sig_shader_trans_zfighting2, 0, 9, SKIP_PATCH, 0);
// These patches forces the decal bias pointers to be explicitly overwritten
// so weird config.txt setups can't break this fix.
Patch(patch_shader_trans_zfighting3a_fix, sig_shader_trans_zfighting3,         0xE, 4, INT_PATCH, &val_DecalZBiasValue);
Patch(patch_shader_trans_zfighting3b_fix, sig_shader_trans_zfighting3,        0x34, 4, INT_PATCH, &val_DecalSlopeZBiasValue);
Patch(patch_shader_trans_zfighting3c_fix, sig_shader_trans_zfighting3,  0xE + 0x50, 4, INT_PATCH, &val_DecalZBiasValue);
Patch(patch_shader_trans_zfighting3d_fix, sig_shader_trans_zfighting3, 0x34 + 0x50, 4, INT_PATCH, &val_DecalSlopeZBiasValue);

void init_shdr_trans_zfighting_fixes(){
    if (patch_shader_trans_zfighting2_fix.build()){
        patch_shader_trans_zfighting2_fix.apply();
    };
    if (patch_shader_trans_zfighting3a_fix.build()
     && patch_shader_trans_zfighting3b_fix.build()
     && patch_shader_trans_zfighting3c_fix.build()
     && patch_shader_trans_zfighting3d_fix.build()){
        patch_shader_trans_zfighting3a_fix.apply();
        patch_shader_trans_zfighting3b_fix.apply();
        patch_shader_trans_zfighting3c_fix.apply();
        patch_shader_trans_zfighting3d_fix.apply();
    };
}

void revert_shdr_trans_zfighting_fixes(){
    patch_shader_trans_zfighting2_fix.revert();
    patch_shader_trans_zfighting3a_fix.revert();
    patch_shader_trans_zfighting3b_fix.revert();
    patch_shader_trans_zfighting3c_fix.revert();
    patch_shader_trans_zfighting3d_fix.revert();
}
