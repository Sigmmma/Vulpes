/*
 * Vulpes (c) 2019 gbMichelle
 *
 * This program is free software under the GNU General Public License v3.0 or later. See LICENSE for more information.
 */

#include <windows.h>

// TODO: Add code that downloads and updates the mod.

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH && GetModuleHandle("Vulpes.dll") == NULL) {
        LoadLibrary("Vulpes.dll");
    }
    return TRUE;
}
