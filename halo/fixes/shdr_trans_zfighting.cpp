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

Signature(false, sig_shader_trans_zfighting3b,
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
Patch(patch_shader_trans_zfighting2_fix);
// These patches forces the decal bias pointers to be explicitly overwritten
// so weird config.txt setups can't break this fix.
Patch(patch_shader_trans_zfighting3a_fix);
Patch(patch_shader_trans_zfighting3b_fix);
Patch(patch_shader_trans_zfighting3c_fix);
Patch(patch_shader_trans_zfighting3d_fix);

void init_shdr_trans_zfighting_fixes(){
    static uintptr_t sig_addr2 = sig_shader_trans_zfighting2.address();
    static uintptr_t sig_addr3 = sig_shader_trans_zfighting3.address();
    static uintptr_t sig_addr4 = sig_shader_trans_zfighting3b.address(sig_addr3+1);
    if (!patch_shader_trans_zfighting2_fix.is_built() && sig_addr2){
        patch_shader_trans_zfighting2_fix.build_old(sig_addr2, 9, NOP_PATCH, 0);
    };
    if (patch_shader_trans_zfighting2_fix.is_built()){
        patch_shader_trans_zfighting2_fix.apply();
    };
    if (!patch_shader_trans_zfighting3a_fix.is_built()
     && !patch_shader_trans_zfighting3b_fix.is_built()
     && !patch_shader_trans_zfighting3c_fix.is_built()
     && !patch_shader_trans_zfighting3d_fix.is_built()
     && sig_addr3 && sig_addr4){
        patch_shader_trans_zfighting3a_fix.build_int(sig_addr3+0xE,  (uint32_t)&val_DecalZBiasValue);
        patch_shader_trans_zfighting3b_fix.build_int(sig_addr3+0x34, (uint32_t)&val_DecalSlopeZBiasValue);
        patch_shader_trans_zfighting3c_fix.build_int(sig_addr4+0xE,  (uint32_t)&val_DecalZBiasValue);
        patch_shader_trans_zfighting3d_fix.build_int(sig_addr4+0x34, (uint32_t)&val_DecalSlopeZBiasValue);
    };
    if (patch_shader_trans_zfighting3a_fix.is_built()
     && patch_shader_trans_zfighting3b_fix.is_built()
     && patch_shader_trans_zfighting3c_fix.is_built()
     && patch_shader_trans_zfighting3d_fix.is_built()){
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
