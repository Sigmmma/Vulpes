/*
 * This file is part of Vulpes, an extension of Halo Custom Edition's capabilities.
 * Copyright (C) 2019-2020 gbMichelle (Michelle van der Graaf)
 *
 * Vulpes is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, version 3.
 *
 * Vulpes is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * long with Vulpes.  If not, see <https://www.gnu.org/licenses/agpl-3.0.en.html>
 */

#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <stdarg.h>

// These functions are and should only be called by the game.
__attribute__((cdecl))
void auto_complete(char* buffer[], uint16_t* current_index) asm("_handler__auto_complete");
__attribute__((cdecl))
bool process_command(char* input) asm("_handler___process_command");

// Interface for registering commands.

enum VulpesArgType {
    A_STRING,

    A_LONG,
    A_SHORT,
    A_CHAR,

    A_FLOAT,
    A_BOOL,
    A_TIME
};

class VulpesArgDef {
public:
    VulpesArgDef(std::string arg_name, bool required, VulpesArgType arg_type);
    VulpesArgDef(std::string arg_name, bool required, VulpesArgType arg_type, int max_char);
    VulpesArgDef(std::string arg_name, bool required, VulpesArgType arg_type, int64_t min, int64_t max);
    VulpesArgDef(std::string arg_name, bool required, VulpesArgType arg_type, float min, float max);

    std::string parse_str(std::string input);
    int32_t parse_int(std::string input, char** leftover);
    float parse_flt(std::string input, char** leftover);
    bool parse_bool(std::string input);
    int parse_time(std::string input, bool* success);

    std::string name;
    std::string display_name;
    VulpesArgType type;
    bool min_max;
    bool optional;
    int64_t imax;
    int64_t imin;
    float fmin = -16777216;
    float fmax = 16777216;
    int max_characters;
private:
    void set_display_name();
};

class VulpesArg {
public:
    VulpesArg(VulpesArgDef def, std::string in, bool* success);

    int         int_out();
    float       flt_out();
    bool       bool_out();
    std::string str_out();
    int      time_ticks();
    int      time_seconds();

    bool     has_output();

private:
    VulpesArgDef* definition;
    std::string input;
    bool output = false;

    int intout;
    float fltout;
    bool boolout;
    std::string strout;
};

class VulpesCommand {
public:
    VulpesCommand(std::string cmd_name,
                  bool (*function_to_exec)(std::vector<VulpesArg>),
                  uint8_t min_dev_level, int num_args, ...);
    ~VulpesCommand();
    std::string get_name();
    char* get_name_chars();
    std::vector<VulpesArgDef> get_arg_defs();
    std::vector<VulpesArg> parse_args(std::vector<std::string> arg_strings, bool* success);
    uint8_t get_dev_level();
    bool execute(std::vector<VulpesArg> parsed_args);

private:
    char name_chars[64];
    std::string name;
    std::vector<VulpesArgDef> args;
    bool (*cmd_func)(std::vector<VulpesArg>);
    uint8_t developer_level;
};
