#pragma once 

#include <iostream>
#include <vector>
#include <string_view>
#include <string>
#include <any>
#include <sstream>
#include <stdexcept>
#include <expected>

#include "types.hpp"

namespace cynlr {

    enum class args_parse_error : u8 {
        invalid_key_value, invalid_value_type, invalid_value_range, invalid_file_format
    };

    enum class arg_key : u8 {
        help, mode, input, delimiter, run_time_s, threshold, input_columns, invalid
    };

    auto str_to_arg_key(cstr key) -> arg_key {
        if (key == "help") { return arg_key::help; };
        if (key == "mode") { return arg_key::mode; };
        if (key == "input") { return arg_key::input; };
        if (key == "input_columns") { return arg_key::input_columns; };
        if (key == "delimiter") { return arg_key::delimiter; };
        if (key == "run_time_s") { return arg_key::run_time_s; };
        if (key == "threshold") { return arg_key::threshold; };
        return arg_key::invalid;
    };

    auto parse_arguments(int argc, char* argv[]) -> std::expected<std::vector<std::pair<arg_key, any>>, args_parse_error> {
        std::vector<std::pair<arg_key, any>> parsed_args;
    
        for (usize i = 1; i < argc; ++i) {
            cstr arg = argv[i];
    
            if (arg.starts_with("--")) {
                arg.remove_prefix(2); // Remove the leading "--"
                auto delimiter_Ppos = arg.find('=');
    
                if (delimiter_Ppos == cstr::npos) {
                    // Argument without a value, e.g., --help
                    parsed_args.emplace_back(str_to_arg_key(arg), "");
                } else {
                    // Argument with a value, e.g., --mode=realtime
                    cstr key = arg.substr(0, delimiter_Ppos);
                    cstr value = arg.substr(delimiter_Ppos + 1);
    
                    // Type conversion based on key, customize as needed
                    if (key == "delimiter" && value.size() == 1) {
                        parsed_args.emplace_back(str_to_arg_key(key), value[0]); // Store as char
                    } else if (key == "run_time_s" || key == "threshold" || key == "input_columns") {
                        try {
                            isize int_value = std::stoi(str(value));
                            parsed_args.emplace_back(str_to_arg_key(key), int_value); // Store as int
                        } catch (const std::exception& e) {
                            return std::unexpected{args_parse_error::invalid_key_value};
                        }
                    } else {
                        parsed_args.emplace_back(str_to_arg_key(key), str(value)); // Store as string
                    }
                }
            }
        }
    
        return parsed_args;
    }

} // namespace cynlr