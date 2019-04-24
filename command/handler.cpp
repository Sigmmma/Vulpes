#include "handler.hpp"
#include "../halo_functions/console.hpp"
#include <cstdio>


char f[] = "fox";
__attribute__((cdecl))
int16_t auto_complete(char** const buffer, int16_t current_index, const char* const input){
    if (current_index < 256){

        buffer[current_index] = f;
        current_index++;
    };
    return current_index;
}
