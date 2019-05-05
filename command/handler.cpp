#include "handler.hpp"
#include "../halo_functions/console.hpp"
#include <regex>
#include <cassert>
#include <algorithm>
#include <exception>
#include <ctype.h>

using namespace std;

class VulpesCommandException_WrongArgCount : public exception {
    virtual const char* what() const throw(){
        return "Input had an invalid argument count.";
    }
} wrong_arg_count_exception;

class VulpesCommandException_UnparsableArg : public exception {
    virtual const char* what() const throw(){
        return "Input had an argument that couldn't be parsed.";
    }
} unparsable_arg_exception;

static vector<VulpesCommand*> commands;
regex command_split("([^\\s\"]+)|\"([^\"]*)\"|\"([^\"]*)");

// Adds Vulpes results to an already constructed autocomplete list.
__attribute__((cdecl))
void auto_complete(char* buffer[], uint16_t* current_index, const char* input){
    string input_str(input);

    int i = 0;
    int j = *current_index;
    while (i < commands.size() && *current_index < 256){
        string cmp_str = commands[i]->get_name();
        if (input_str.size() <= cmp_str.size()){
            cmp_str.resize(input_str.size());
            if (input_str == cmp_str){
                buffer[j] = commands[i]->get_name().data();
                j++;
            };
        };
        i++;
    };
    *current_index = j;
}

// Returns false if the original Halo function should not fire after this.
__attribute__((cdecl))
bool process_command(char* input, int32_t network_machine_id){
    string input_str(input);
    // A line starting with this should be ignored.
    if(input_str.find(";") == 0){
        return false;
    };
    if(input_str.find("#") == 0){
        return false;
    };
    if(input_str.find("//") == 0){
        return false;
    };

    vector<string> matches;

    smatch arg_match;
    string::const_iterator search_start(input_str.cbegin());
    while(regex_search(search_start, input_str.cend(), arg_match, command_split)){
        if(arg_match[1] != ""){
            string str = arg_match[1];
            int semicolon_pos = str.find(";");
            if (semicolon_pos != string::npos){
                if (semicolon_pos > 0){
                    matches.push_back(str.substr(0, semicolon_pos));
                };
                break; // We shouldn't match anything after a semicolon
            };
            matches.push_back(str);
        }else if(arg_match[2] != ""){
            matches.push_back(arg_match[2]);
        }else if(arg_match[3] != ""){
            matches.push_back(arg_match[3]);
        };
        search_start = arg_match.suffix().first;
    };

    string cmd_to_find = matches[0];
    matches.erase(matches.begin());
    transform(cmd_to_find.begin(), cmd_to_find.end(), cmd_to_find.begin(), ::tolower);

    VulpesCommand* matching_cmd;
    bool found_match = false; // this is used because checking if the pointer
                              // for matching_cmd !=0 has weird behavior.

    for (int i=0; i<commands.size(); i++){
        if (commands[i]->get_name() == cmd_to_find){
            matching_cmd = commands[i];
            found_match = true;
            break;
        };
    };
    if (found_match){
        vector<VulpesArg> parsed_args;
        bool success = true;
        try {
            parsed_args = matching_cmd->parse_args(matches, &success);
        }catch (exception& e){
            console_out_error("Couldn't parse command.");
            console_out_error(e.what());
            return false;
        };
        if (success){
            matching_cmd->execute(parsed_args);
        };
        return false;
    };
    return true;
}

VulpesArgDef::VulpesArgDef(string arg_name, bool is_optional, VulpesArgType arg_type){
    type = arg_type;
    name = arg_name;
    optional = is_optional;
    min_max = false;

    if (arg_type==LONG){
        imin = -2147483647;
        imax = 2147483647;
    } else if(arg_type==SHORT){
        imin = -32767;
        imax = 32767;
    } else if(arg_type==CHAR){
        imin = -127;
        imax = 127;
    } else if(arg_type==TIME){
        imin = 0;
        imax = 2147483647;
    };

    set_display_name();
}

VulpesArgDef::VulpesArgDef(string arg_name, bool is_optional, VulpesArgType arg_type, int max_char){
    assert(arg_type==STRING);
    type = arg_type;
    name = arg_name;
    optional = is_optional;
    max_characters = max_char;
    min_max = false;

    set_display_name();
}

VulpesArgDef::VulpesArgDef(string arg_name, bool is_optional, VulpesArgType arg_type, int64_t min, int64_t max){
    assert(arg_type==LONG || arg_type==SHORT || arg_type==CHAR || arg_type==TIME);
    type = arg_type;
    name = arg_name;
    optional = is_optional;
    min_max = true;

    if (type==LONG){
        assert(min >= -2147483647);
        assert(max <= 2147483647);
    } else if (type==SHORT){
        assert(min >= -32767);
        assert(max <= 32767);
    } else if (type==CHAR){
        assert(min >= -127);
        assert(max <= 127);
    };
    imin = min;
    imax = max;

    set_display_name();
}

VulpesArgDef::VulpesArgDef(string arg_name, bool is_optional, VulpesArgType arg_type, float min, float max){
    assert(arg_type==FLOAT);
    type = arg_type;
    name = arg_name;
    optional = is_optional;
    min_max = true;
    fmin = min;
    fmax = max;

    set_display_name();
}

string VulpesArgDef::parse_str(string input){
    if (max_characters && input.size() > max_characters){
        return input.substr(0, max_characters);
    };
    return input;
}

int64_t parse_to_integer(string input, char** leftover){
    int64_t output;
    if(input.find("0x") == 0){
        output = strtol(input.substr(2, input.size()).data(), leftover, 16);
    }else{
        output = strtol(input.data(), leftover, 10);
    };
    return output;
}

int32_t VulpesArgDef::parse_int(string input, char** leftover){
    int32_t output;
    int64_t parsed = parse_to_integer(input, leftover);
    char error_str[80];
    if(parsed > imax){
        snprintf(error_str, 80,
                "Input for arg %s too high. Highest allowed: %d, got %d.",
                name.data(), imax, parsed);
        console_out_error(error_str);
        snprintf(error_str, 80,
                "Setting it to %d.",
                imax);
        console_out_error(error_str);
        output = imax;
    }else if(parsed < imin){
        snprintf(error_str, 80,
                "Input for arg %s too low. Lowest allowed: %d, got %d.",
                name.data(), imin, parsed);
        console_out_error(error_str);
        snprintf(error_str, 80,
                "Setting it to %d.",
                imax);
        console_out_error(error_str);
        output = imin;
    }else{
        output = static_cast<int32_t>(parsed);
    };
    return output;
}

float VulpesArgDef::parse_flt(string input, char** leftover){
    float output = strtof(input.data(), leftover);
    char error_str[80];
    if(output > fmax){
        snprintf(error_str, 80,
                "Input for arg %s too high. Highest allowed: %f, got %f.",
                name.data(), fmax, output);
        console_out_error(error_str);
        snprintf(error_str, 80,
                "Setting it to %f.",
                fmax);
        console_out_error(error_str);
        output = fmax;
    }else if(output < fmin){
        snprintf(error_str, 80,
                "Input for arg %s too low. Lowest allowed: %f, got %f.",
                name.data(), fmin, output);
        console_out_error(error_str);
        snprintf(error_str, 80,
                "Setting it to %f.",
                fmax);
        console_out_error(error_str);
        output = fmin;
    };
    return output;
}

bool VulpesArgDef::parse_bool(string input){
    string parsed = input;
    transform(parsed.begin(), parsed.end(), parsed.begin(), ::tolower);
    if (input == "yes" || input == "on" || input == "true" || input == "1"){
        return true;
    }else if (input == "no" || input == "off" || input == "false" || input == "0"){
        return false;
    }else {
        char error_str[80];
        snprintf(error_str, 80,
                "Couldn't parse input \"%s\" for %s. Assuming false.",
                input.data(), name.data());
        console_out(error_str);
        return false;
    };
}

enum class Time {
    TICKS,
    SECONDS,
    MINUTES,
    HOURS,
    DAYS,
    TBD
};

regex time_split("([\\d:]+)\\s*([a-zA-Z]+)");
regex colon_split("([^:]+)");

int VulpesArgDef::parse_time(string input, bool* success){
    int64_t output = 0;

    // Check if we got anything parsable.
    const char* input_a = input.data();
    bool has_alpha_chars = false;
    bool has_digit_chars = false;
    int i = 0;
    while (input_a[i]){
        if (isalpha(input_a[i])){
            has_alpha_chars = true;
        }else if (isdigit(input_a[i])){
            has_digit_chars = true;
        };
        i++;
    };

    // Throw a fit if our input can't be valid.
    char error_str[80];
    if (!has_digit_chars && !has_alpha_chars){
        printf("Nothing.");
        snprintf(error_str, 80,
                "Couldn't parse input \"%s\" for %s. What!?",
                input.data(), name.data());
        console_out_error(error_str);
        *success = false;
        return 0;
    }else if (!has_digit_chars){
        printf("All letters.");
        snprintf(error_str, 80,
                "Couldn't parse input \"%s\" for %s. Numbers please?",
                input.data(), name.data());
        console_out_error(error_str);
        *success = false;
        return 0;
    };

    // Split the input into numbers and units if we got both digits and alphabetics.
    vector<string> n;
    vector<string> a;
    if (has_alpha_chars && has_digit_chars){
        printf("Digits and Alphabetics.");
        smatch time_match;
        string::const_iterator search_start(input.cbegin());
        while(regex_search(search_start, input.cend(), time_match, time_split)){
            n.push_back(time_match[1]);
            a.push_back(time_match[2]);

            search_start = time_match.suffix().first;
        };
    }else if (has_digit_chars){
        printf("Just digits.");
        n.push_back(input);
        a.push_back(string(""));
    };
    // Throw more fits if the splitting failed.
    if (!n.size()){
        snprintf(error_str, 80,
                "Couldn't parse input \"%s\" for %s. I got nothing.",
                input.data(), name.data());
        console_out(error_str);
        *success = false;
        return 0;
    };
    // Get all integers in each string in n.
    vector<vector<int>> n_int;
    for (i = 0; i < n.size(); i++){
        vector<int> numbers;
        string num_str = n[i];
        smatch num_match;
        string::const_iterator search_start(num_str.cbegin());
        while(regex_search(search_start, num_str.cend(), num_match, colon_split)){
            char* dummy;
            numbers.push_back(strtol(string(num_match[1]).data(), &dummy, 10));
            search_start = num_match.suffix().first;
        };
        n_int.push_back(numbers);
    };
    // Turn all our intergers into their appropriate tick counts.
    for (i = 0; i < n_int.size() && i < a.size(); i++){
        transform(a[i].begin(), a[i].end(), a[i].begin(), ::tolower);
        Time unit;
        if (a[i] == ""){
            if (n_int[i].size() == 1){
                unit = Time::SECONDS;
            } else if(n_int[i].size() == 2){
                unit = Time::MINUTES;
            } else if(n_int[i].size() == 3){
                unit = Time::HOURS;
            } else if(n_int[i].size() == 4){
                unit = Time::DAYS;
            } else {
                snprintf(error_str, 80,
                        "Couldn't parse input \"%s\" for %s. Too few or too many units.",
                        input.data(), name.data());
                console_out_error(error_str);
                *success = false;
                return 0;
            };
        } else if (a[i] == "t" || a[i] == "tick" || a[i] == "ticks"){
            unit = Time::TICKS;
        } else if (a[i] == "s" || a[i] == "sec" || a[i] == "secs"
                || a[i] == "second" || a[i] == "seconds"){
            unit = Time::SECONDS;
        } else if (a[i] == "s" || a[i] == "sec" || a[i] == "secs"
                || a[i] == "second" || a[i] == "seconds"){
            unit = Time::MINUTES;
        } else if (a[i] == "h" || a[i] == "hour" || a[i] == "hours"){
            unit = Time::HOURS;
        } else if (a[i] == "d" || a[i] == "day" || a[i] == "days"){
            unit = Time::DAYS;
        } else {
            snprintf(error_str, 80,
                    "Couldn't parse input \"%s\" for %s. %s is not an accepted time unit.",
                    input.data(), name.data(), a[i].data());
            console_out_error(error_str);
            *success = false;
            return 0;
        };
        vector<int> current_set = n_int[i];
        for (int j=0; j < current_set.size(); j++){
            // Lower the unit for the next parse.
            switch (unit){
                case Time::DAYS :
                    output += current_set[j] * 24 * 60 * 60 * 30;
                    unit = Time::HOURS;
                    break;
                case Time::HOURS :
                    output += current_set[j] * 60 * 60 * 30;
                    unit = Time::MINUTES;
                    break;
                case Time::MINUTES :
                    output += current_set[j] * 60 * 30;
                    unit = Time::SECONDS;
                    break;
                case Time::SECONDS :
                    output += current_set[j] * 30;
                    unit = Time::TICKS;
                    break;
                case Time::TICKS :
                    output += current_set[j];
                    break;
            };
        };
    };
    *success = true;
    return static_cast<int>(output);
}

void VulpesArgDef::set_display_name(){
    if (optional){
        display_name = name + "[";
    }else{
        display_name = name + "<";
    };

    switch (type){
        case STRING :
            display_name = display_name + "string";
            goto end;
        case LONG :
            display_name = display_name + "long";
            goto integer;
        case SHORT :
            display_name = display_name + "short";
            goto integer;
        case CHAR :
            display_name = display_name + "byte";
            goto integer;
        case FLOAT :
            display_name = display_name + "float";
            goto floating_point;
        case BOOL :
            display_name = display_name + "on-off";
            goto end;
        case TIME :
            display_name = display_name + "time";
            goto end;
    };

    integer:
    if (min_max){
        display_name = display_name + "(" + to_string(imin) + "-" + to_string(imax) + ")";
    };
    goto end;

    floating_point:
    if (min_max){
        display_name = display_name + "(" + to_string(fmin) + "-" + to_string(fmax) + ")";
    };

    end:
    if (optional){
        display_name = display_name + "]";
    }else{
        display_name = display_name + ">";
    };
}

VulpesArg::VulpesArg(VulpesArgDef def, std::string in, bool* success){
    definition = &def;
    input = in;
    VulpesArgType type = definition->type;

    if (type != STRING && input == ""){
        output = false;
        return;
    };
    char* leftover;
    switch (type){
            case STRING :
            strout = definition->parse_str(input);
            output = true;
            break;
        case LONG :
        case SHORT :
        case CHAR :
            intout = definition->parse_int(input, &leftover);
            output = true;
            if (*leftover != '\0'){
                *success = false;
            };
            break;
        case FLOAT :
            fltout = definition->parse_flt(input, &leftover);
            output = true;
            if (*leftover != '\0'){
                *success = false;
            };
            break;
        case BOOL :
            boolout = definition->parse_bool(input);
            output = true;
            break;
        case TIME :
            intout = definition->parse_time(input, success);
            output = true;
            break;
    };
}

int VulpesArg::int_out(){
    return intout;
}
float VulpesArg::flt_out(){
    return fltout;
}
bool VulpesArg::bool_out(){
    return boolout;
}
string VulpesArg::str_out(){
    return strout;
}
int VulpesArg::time_ticks(){
    return intout;
}
int VulpesArg::time_seconds(){
    return (intout/30);
}
bool VulpesArg::has_output(){
    return output;
}

VulpesCommand::VulpesCommand(string cmd_name,
                             bool (*function_to_exec)(vector<VulpesArg>),
                             int num_args, ...){
    name = cmd_name;
    cmd_func = function_to_exec;
    va_list va_args;
    va_start(va_args, num_args);
    for (int i=0; i<num_args; i++){
        args.push_back(va_arg(va_args, VulpesArgDef));
    };
    va_end(va_args);
    commands.push_back(this);
    printf("Command %s added at index %d\n",
           commands[commands.size()-1]->get_name().data(), commands.size()-1);
}

// Remove command from the command list on destruction.
VulpesCommand::~VulpesCommand(){
    for (int i=0;i<commands.size();i++){
        if (commands[i] == this){
            printf("Removing command %s by erasing index %d which contains: %s\n",
                   name, i, commands[i]->get_name());
            commands.erase(commands.begin() + i);
            break;
        };
    };
}

string VulpesCommand::get_name(){
    return name;
}

vector<VulpesArgDef> VulpesCommand::get_arg_defs(){
    return args;
}

vector<VulpesArg> VulpesCommand::parse_args(vector<string> arg_strings, bool* success){
    char error_str[80];
    int required_args;
    for (required_args = 0; required_args<args.size(); required_args++){
        if (args[required_args].optional){
            break;
        };
    };
    if (arg_strings.size() < required_args){
        snprintf(error_str, 80,
                "Too few args for command: %s. Got %d, expected %d to %d.",
                name.data(), arg_strings.size(), required_args, args.size());
        console_out(error_str);
        throw wrong_arg_count_exception;
    }else if (arg_strings.size() > args.size()){
        snprintf(error_str, 80,
                "Too many args for command: %s. Got %d, expected %d to %d.",
                name.data(), arg_strings.size(), required_args, args.size());
        console_out(error_str);
        throw wrong_arg_count_exception;
    };
    vector<VulpesArg> parsed_args;
    for (int i=0; i<arg_strings.size(); i++){
        parsed_args.push_back(VulpesArg(args[i], arg_strings[i], success));
        if (!*success){
            snprintf(error_str, 80,
                    "Couldn't parse arg #%d %s",
                    i+1, args[i].display_name.data());
            console_out_error(error_str);
            *success = false;
            break;
        };
    };
    return parsed_args;
}

bool VulpesCommand::execute(vector<VulpesArg> parsed_args){
    bool success;
    try {
        success = cmd_func(parsed_args);
    }catch(exception& e){
        char error_str[80];
        snprintf(error_str, 80,
                "Execution of command %s failed. Exception: %s",
                name.data(), e.what());
        success = false;
    };
    return success;
}
