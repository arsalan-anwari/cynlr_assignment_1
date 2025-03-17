#pragma once

#include <concepts>
#include <vector>
#include <fstream>
#include <sstream>
#include <expected>
#include <span>
#include <array>
#include <algorithm>
#include <regex>

#include "settings.hpp"
#include "types.hpp"
#include "logging.hpp"
#include "traits/type_format.hpp"

namespace cynlr {

    enum class csv_reader_error : u8 {
        invalid_file_name, invalid_data_format, invalid_data_range
    };

    template<typename T>
    struct csv_reader {

        csv_reader(cstr file_name, isize column_size, char delimiter = ',') :
            file_name(file_name),
            column_size(column_size),
            delimiter(delimiter)
        {
            row_data.reserve(column_size);
            data.reserve(column_size * settings::MinCsvReaderBufferSize);
        }

        auto parse() -> std::expected<std::span<T>, csv_reader_error> {
            std::ifstream file(file_name.data());
        
            if (!file.is_open()) {
                Log(std::format("Error: Could not open file {}", file_name));
                return std::unexpected{csv_reader_error::invalid_file_name};
            }
    
            std::string line;
            while (std::getline(file, line)) {  // Read each row
                std::stringstream ss(line);
                row_data.clear();
                usize col = 0;

                while (col < column_size) {
                    std::string token;
                    if (!std::getline(ss, token, delimiter)) {  // Read token
                        break;
                    }

                    // Validate token using the custom function
                    if (!is_valid_token(token)) {
                        Log(std::format("Error: Invalid data format! Could not convert element {} to type {}", token, type_format_trait::type_info<T>::name()));
                        return std::unexpected{csv_reader_error::invalid_data_format};
                    }

                    // Convert token to a floating-point value or other type
                    T value;
                    try {
                        if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>) {
                            value = std::stod(token);  // For floating-point types
                        } else {
                            value = static_cast<T>(std::stoi(token));  // For integers or other types
                        }
                    } catch (const std::exception& e) {
                        Log(std::format("Error: Invalid data format! Could not convert element {} to type {}", token, type_format_trait::type_info<T>::name()));
                        return std::unexpected{csv_reader_error::invalid_data_format};
                    }

                    // Ensure the value is within the valid range of `T`
                    if (value < std::numeric_limits<T>::min() || value > std::numeric_limits<T>::max()) {
                        Log(std::format("Error: Data not in range of type {}", type_format_trait::type_info<T>::name()));
                        return std::unexpected{csv_reader_error::invalid_data_range};
                    }

                    row_data[col++] = value;  // Store value in the array
                }

                // Append entire row to `data`
                data.insert(data.end(), row_data.begin(), row_data.begin() + col);
            }


            file.close();
            return data;
        };
    
    private:
        std::vector<T> data;
        std::vector<T> row_data;

        cstr file_name;
        isize column_size;
        char delimiter;

        bool is_valid_token(const std::string& token) {
            static const std::regex float_or_int_pattern(R"(^[+-]?(\d+(\.\d*)?|\.\d+)([eE][+-]?\d+)?$)");
            return std::regex_match(token, float_or_int_pattern);
        }

    };


} // namespace cynlr