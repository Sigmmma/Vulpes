#pragma once
// Function to preserve registers.
__attribute__((naked))
void save_registers(){
    asm(
    "save_registers:\n"
        "push eax;\n"
        "push ebx;\n"
        "push ecx;\n"
        "push edx;\n"
        "ret;\n"
    );
}
// Function to restore registers.
__attribute__((naked))
void restore_registers(){
    asm(
    "restore_registers:\n"
        "pop edx;\n"
        "pop ecx;\n"
        "pop ebx;\n"
        "pop eax;\n"
        "ret;\n"
    );
}
