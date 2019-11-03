#define WIN32_MEAN_AND_LEAN
#include <windows.h>

// TODO: Add code that downloads and updates the mod.

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH && GetModuleHandle("Vulpes.dll") == NULL) {
        LoadLibrary("Vulpes.dll");
    }
    return TRUE;
}
