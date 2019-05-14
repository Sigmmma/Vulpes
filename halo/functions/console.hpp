#pragma once
/*
* Copied and edited from the Chimera 2017 repo. MIT License
*/
#include <string>

#include "../memory/types.hpp"

/// Output a C string to the console while optionally using a color.
void console_out(const char *text, const ARGBFloat &color = ARGBFloat()) noexcept;

/// Output a C++ string to the console with a C++ string while optionally using a color.
void console_out(const std::string &text, const ARGBFloat &color = ARGBFloat()) noexcept;

/// Output a C string to the console using yellow text. This is used for warnings.
void console_out_warning(const char *text) noexcept;

/// Output a C++ string to the console using yellow text. This is used for warnings.
void console_out_warning(const std::string &text) noexcept;

/// Output a C string to the console using red text. This is used for errors.
void console_out_error(const char *text) noexcept;

/// Output a C++ string to the console using red text. This is used for errors.
void console_out_error(const std::string &text) noexcept;
