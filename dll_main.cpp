#include "vulpes.hpp"
#define WIN32_MEAN_AND_LEAN
#include <windows.h>

static bool loaded = false;

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    if(fdwReason == DLL_PROCESS_ATTACH && !loaded) {
        loaded = true;
        init_vulpes();
    }else if(fdwReason == DLL_PROCESS_DETACH && loaded) {
        loaded = false;
        destruct_vulpes();
    };
    return true;
}
