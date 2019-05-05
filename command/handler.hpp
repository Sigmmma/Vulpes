#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <stdarg.h>

// These functions are and should only be called by the game.
__attribute__((cdecl))
void auto_complete(char* buffer[], uint16_t* current_index, const char* input);
__attribute__((cdecl))
bool process_command(char* input, int32_t network_machine_id);

// Interface for registering commands.

enum VulpesArgType {
    STRING,

    LONG,
    SHORT,
    CHAR,

    FLOAT,
    BOOL,
    TIME
};

class VulpesArgDef {
public:
    VulpesArgDef(std::string arg_name, bool is_optional, VulpesArgType arg_type);
    VulpesArgDef(std::string arg_name, bool is_optional, VulpesArgType arg_type, int max_char);
    VulpesArgDef(std::string arg_name, bool is_optional, VulpesArgType arg_type, int64_t min, int64_t max);
    VulpesArgDef(std::string arg_name, bool is_optional, VulpesArgType arg_type, float min, float max);

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
                  int num_args, ...);
    ~VulpesCommand();
    std::string get_name();
    std::vector<VulpesArgDef> get_arg_defs();
    std::vector<VulpesArg> parse_args(std::vector<std::string> arg_strings, bool* success);

    bool execute(std::vector<VulpesArg> parsed_args);

private:
    std::string name;
    std::vector<VulpesArgDef> args;
    bool (*cmd_func)(std::vector<VulpesArg>);
};
