#include "../../hooker/hooker.hpp"
#include "console.hpp"
/*
* Copied and edited from the Chimera 2017 repo. MIT License
*/

Signature(true, sig_console_out,
    {0x83, 0xEC, 0x10, 0x57, 0x8B, 0xF8, 0xA0, -1, -1, -1, -1, 0x84, 0xC0, 0xC7, 0x44, 0x24, 0x04, 0x00, 0x00, 0x80, 0x3F});

bool silence_all_messages = false;

const char *current_command = nullptr;

void console_out(const char *text, const ARGBFloat &color) noexcept {
    if(silence_all_messages) return;
    std::string formatted_output = current_command ? (std::string(current_command) + ": " + text) : text;
    static intptr_t console_out_address = sig_console_out.get_address();
    asm (
        "pushad;"
        "mov eax, %0;"
        "push %1;"
        "push %2;"
        "call %3;"
        "add esp, 8;"
        "popad;"
        :
        : "r" (&color), "r" (formatted_output.data()), "r" ("%s"), "r" (console_out_address)
    );
}

void console_out(const std::string &text, const ARGBFloat &color) noexcept {
    return console_out(text.data(), color);
}

void console_out_warning(const char *text) noexcept {
    console_out(text, ARGBFloat(1.0, 0.94, 0.75, 0.098));
}

void console_out_warning(const std::string &text) noexcept {
    return console_out_warning(text.data());
}

void console_out_error(const char *text) noexcept {
    bool already_silent = silence_all_messages;
    silence_all_messages = false;
    console_out(text, ARGBFloat(1.0, 1.0, 0.2, 0.2));
    silence_all_messages = already_silent;
}

void console_out_error(const std::string &text) noexcept {
    return console_out_error(text.data());
}
