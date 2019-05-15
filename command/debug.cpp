#include "debug.hpp"
#include "handler.hpp"
#include "../halo/functions/console.hpp"
#include "../halo/fixes/shdr_trans_zfighting.hpp"

bool toggle_shader_trans_fix(std::vector<VulpesArg> input){
    bool on = input[0].bool_out();
    if (on){
        init_shdr_trans_zfighting_fixes();
        console_out("Turned ON transparent fix.");
    }else{
        revert_shdr_trans_zfighting_fixes();
        console_out("Turned OFF transparent fix.");
    };
    return true;
}

void init_debug_commands(){
    static VulpesCommand cmd_dev_shader_transparent_fix(
        "v_dev_shader_transparent_fix", &toggle_shader_trans_fix, 4, 1,
        VulpesArgDef("", true, A_BOOL)
    );
}
