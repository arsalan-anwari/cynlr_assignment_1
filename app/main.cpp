#include <iostream>
#include <iomanip>
#include <optional>
#include <algorithm>
#include <numeric> // for accumulate

#include "args.hpp"

#include "data_pool.hpp"
#include "task.hpp"
#include "types.hpp"
#include "scheduler.hpp"
#include "csv_reader.hpp"
#include "algorithm.hpp"

using namespace cynlr;

// Function to calculate the mean
template <typename T>
f64 calculate_mean(const std::vector<T>& vec) {
    return static_cast<f64>(std::accumulate(vec.begin(), vec.end(), T(0))) / vec.size();
}

// Function to calculate the median
template <typename T>
f64 calculate_median(std::vector<T> vec) {
    std::sort(vec.begin(), vec.end());
    size_t n = vec.size();
    
    if (n % 2 == 0) {
        return (static_cast<f64>(vec[n / 2 - 1]) + static_cast<f64>(vec[n / 2])) / 2.0;
    } else {
        return static_cast<f64>(vec[n / 2]);
    }
}

// Function to find min and max
template <typename T>
std::pair<usize, usize> find_min_max(const std::vector<T>& vec) {
    auto [minIt, maxIt] = std::minmax_element(vec.begin(), vec.end());
    return {*minIt, *maxIt};
}


void print_available_commands() {
    std::cout << R"""(
Available Commands:
--------------------
--help                 : Display help information.
--mode=<value>         : Set the mode of operation. Available values: debug, testing, profiling.
--input=<csv_file>.csv : Specify the input file. Only .csv files are allowed.
--delimiter=<char>     : Set the delimiter character (e.g., ';').
--run_time_s=<int>     : Specify the run time in seconds.
--input_columns=<int>  : Number of columns in input file.
--threshold=<u8>       : Threshold to use between [0 ... 256].

Usage:
[command] [--option=value] [...]

Examples:
[command] --help

[command] --mode=debug --run_time_s=10 --threshold=175

[command] --mode=profiling --run_time_s=10 --threshold=175

[command] --mode=testing --run_time_s=10 --threshold=175 --input_columns=32 --delimiter="," --input="file.csv" 
            )""" << std::endl;
}


isize find_key(const std::vector<std::pair<arg_key, any>>& commands, arg_key keyToCheck) {
    for (usize i = 0; i < commands.size(); ++i) {
        if (commands[i].first == keyToCheck) {
            return static_cast<isize>(i); // Return the index if the key is found
        }
    }
    return -1; // Return -1 if the key does not exist
}


std::expected<std::pair<isize, u8>, args_parse_error> check_input_mode_debug_profile(const std::vector<std::pair<arg_key, any>>& commands){
    
    isize run_time_s_idx = find_key(commands, arg_key::run_time_s);
    isize threshold_idx = find_key(commands, arg_key::threshold);

    if ((run_time_s_idx < 0 || threshold_idx < 0)){
        return std::unexpected{args_parse_error::invalid_value_type};
    }

    isize run_time_s = std::any_cast<isize>(commands[run_time_s_idx].second);
    isize threshold = std::any_cast<isize>(commands[threshold_idx].second);

    if((threshold < 0 || threshold > 255)){
        return std::unexpected{args_parse_error::invalid_value_range};
    }

    return std::pair<isize, u8>{run_time_s, static_cast<u8>(threshold)};
}

struct testing_mode_settings {
    isize run_time_s;
    u8 threshold;
    str input;
    isize input_columns;
    char delimiter;
};

std::expected<testing_mode_settings, args_parse_error> check_input_mode_testing(const std::vector<std::pair<arg_key, any>>& commands){
    
    isize run_time_s_idx = find_key(commands, arg_key::run_time_s);
    isize threshold_idx = find_key(commands, arg_key::threshold);
    isize input_idx = find_key(commands, arg_key::input);
    isize input_columns_idx = find_key(commands, arg_key::input_columns);
    isize delimiter_idx = find_key(commands, arg_key::delimiter);

    if ((run_time_s_idx < 0 || threshold_idx < 0 || input_idx < 0 || delimiter_idx < 0 || input_columns_idx < 0)){
        return std::unexpected{args_parse_error::invalid_value_type};
    }

    if ((run_time_s_idx < 0 || threshold_idx < 0 || input_columns_idx < 0)){
        return std::unexpected{args_parse_error::invalid_value_type};
    }

    isize run_time_s = std::any_cast<isize>(commands[run_time_s_idx].second);
    isize threshold = std::any_cast<isize>(commands[threshold_idx].second);
    str input = std::any_cast<str>(commands[input_idx].second);
    isize input_columns = std::any_cast<isize>(commands[input_columns_idx].second);
    char delimiter = std::any_cast<char>(commands[delimiter_idx].second);

    if(threshold < 0 || threshold > 255){
        return std::unexpected{args_parse_error::invalid_value_range};
    }

    if(input_columns <= 0){
        return std::unexpected{args_parse_error::invalid_value_range};
    }

    if (!input.ends_with(".csv")){
        return std::unexpected{args_parse_error::invalid_file_format};
    }
    return testing_mode_settings{run_time_s, static_cast<u8>(threshold), input, input_columns, delimiter};
}


std::array<f32, 9> filter_weights = {0.00025177, 0.008666992, 0.078025818, 0.24130249, 0.343757629, 0.24130249, 0.078025818, 0.008666992, 0.000125885};

void mode_debug(isize application_run_time_s, u8 threshold){
    static constexpr pool_type data_pool_mode = pool_type::producer_consumer_debug_static;
    
    using data_pool_type = data_pool<data_pool_mode, u8, 32u, 2u>;
    data_pool_type pool;
    
    task_data_generator<task_mode::debug, data_pool_type> task1(pool);
    task_filter_threshold<task_mode::debug, data_pool_type, f32, u8, 4u> task2(pool, filter_weights, threshold);

    scheduler sched;
    sched.add_task(task1);
    sched.add_task(task2);

    std::this_thread::sleep_for(std::chrono::seconds(application_run_time_s));

    sched.stop();
}


void mode_profile(isize application_run_time_s, u8 threshold){
    static constexpr pool_type data_pool_mode = pool_type::producer_consumer_profiling_static;
    
    using data_pool_type = data_pool<data_pool_mode, u8, 32u, 2u>;
    data_pool_type pool;

    task_data_generator<task_mode::profiling, data_pool_type> task1(pool);
    task_filter_threshold<task_mode::profiling, data_pool_type, f32, u8, 4u> task2(pool, filter_weights, threshold);

    scheduler sched;
    sched.add_task(task1);
    sched.add_task(task2);

    std::this_thread::sleep_for(std::chrono::seconds(application_run_time_s));

    std::cout << "Waiting for scheduler..." << std::endl;
    sched.stop();

    std::this_thread::sleep_for(std::chrono::seconds(application_run_time_s));
    
    auto producer_durations = pool.tx_durations;
    auto consumer_durations = pool.rx_durations;

    f64 mean_1 = calculate_mean(producer_durations);
    f64 median_1 = calculate_median(producer_durations);
    auto [min_1, max_1] = find_min_max(producer_durations);

    f64 mean_2 = calculate_mean(consumer_durations);
    f64 median_2 = calculate_median(consumer_durations);
    auto [min_2, max_2] = find_min_max(consumer_durations);

    std::cout << std::format(
        "producer_durations [n = {}, mean = {}, median = {}, min = {}, max = {}]: ",
        producer_durations.size(),
        mean_1, median_1, min_1, max_1
    ); 
    size_t i = 0;
    for (const auto& x: producer_durations){
        if (i % 32 == 0) std::cout << std::endl;
        std::cout << std::setw(3) << x << " ";
        i++;
    }

    std::cout << std::endl << std::endl;

    std::cout << std::format(
        "consumer_durations [n = {}, mean = {}, median = {}, min = {}, max = {}]: ",
        consumer_durations.size(),
        mean_2, median_2, min_2, max_2
    ); 
    size_t j = 0;
    for (const auto& x: consumer_durations){
        if (j % 32 == 0) std::cout << std::endl;
        std::cout << std::setw(3) << x << " ";
        j++;
    }

}

void mode_testing(isize application_run_time_s, u8 threshold, str file, isize columns, char delimiter){
    static constexpr pool_type data_pool_mode = pool_type::producer_consumer_single_direction_static;

    using data_pool_type = data_pool<data_pool_mode, u8, 32u, 2u>;
    data_pool_type pool;

    csv_reader<u8> reader(file, columns, delimiter);
    auto result = reader.parse();
    std::span<const u8> tx_data; 
    if(result.has_value()){
        tx_data = result.value();
    }else{
        std::cout << "Error file: " << file << ". Exiting...." << std::endl;
        return;
    }
    
    task_data_generator<task_mode::testing, data_pool_type> task1(pool, tx_data);
    task_filter_threshold<task_mode::testing, data_pool_type, f32, u8, 4u> task2(pool, filter_weights, threshold);

    scheduler sched;
    sched.add_task(task1);
    sched.add_task(task2);

    std::this_thread::sleep_for(std::chrono::seconds(application_run_time_s));

    std::cout << "Waiting for scheduler..." << std::endl;
    sched.stop();

    std::this_thread::sleep_for(std::chrono::seconds(application_run_time_s));

    const auto thresholded_data = task2.get_threshold_data();
    const auto filtered_data = task2.get_filtered_data();

    usize print_iterations = clamp_top(tx_data.size(), 32);
    for(usize iteration = 0; iteration < print_iterations; iteration++){
        usize remainder = tx_data.size() - (32 * iteration);
        
        usize size = remainder < 32 ? remainder : 32;
        usize start = iteration * 32;

        std::cout << "Original [" << size << "]: "; 
        for(usize i = start; i < start + size; i++){
            std::cout << std::setw(3) << static_cast<int>(tx_data[i]) << " ";
        }
        std::cout << std::endl;

        std::cout << "Filtered [" << size << "]: "; 
        for(size_t i = start; i < start + size; i++){
            std::cout << std::setw(3) << static_cast<int>(filtered_data[i]) << " ";
        }
        std::cout << std::endl;

        std::cout << "Treshold [" << size << "]: "; 
        for(size_t i = start; i < start + size; i++){
            std::cout << std::setw(3) << static_cast<int>(thresholded_data[i]) << " ";
        }

        std::cout << std::endl << std::endl;
    }

}

int main(int argc, char* argv[]){
    auto result = parse_arguments(argc, argv);

    if(result.has_value()){
        auto commands = result.value();
        arg_key first_command = commands[0].first;
        switch(first_command){
            case arg_key::help : {
                print_available_commands();
                return 0;
            }

            case arg_key::mode : {
                str mode = std::any_cast<str>(commands[0].second);

                if (mode == str("debug")){
                    
                    auto result = check_input_mode_debug_profile(commands);
                    if (!result.has_value()){
                        auto error = result.error();
                        if (error == args_parse_error::invalid_value_type)
                            std::cout << "Invalid input provided for mode debug!" << std::endl;
                        else if(error == args_parse_error::invalid_value_range)
                            std::cout << "Invalid input range for key threshold!" << std::endl;

                        return 0;
                    }
                    auto [run_time_s, threshold] = result.value();
                    mode_debug(run_time_s, threshold);
                } else if (mode == str("profiling")){
                    
                    auto result = check_input_mode_debug_profile(commands);
                    if (!result.has_value()){
                        auto error = result.error();
                        if (error == args_parse_error::invalid_value_type)
                            std::cout << "Invalid settings provided for mode profile!" << std::endl;
                        else if(error == args_parse_error::invalid_value_range)
                            std::cout << "Invalid input range for key threshold!" << std::endl;
                        
                        return 0;
                    }
                    auto [run_time_s, threshold] = result.value();
                    mode_profile(run_time_s, threshold);
                } else if (mode == str("testing")){
                    auto result = check_input_mode_testing(commands);

                    if (!result.has_value()){
                        auto error = result.error();
                        if (error == args_parse_error::invalid_value_type)
                            std::cout << "Invalid settings provided for mode profile!" << std::endl;
                        else if(error == args_parse_error::invalid_value_range)
                            std::cout << "Invalid input range for keys given!" << std::endl;
                        else if(error == args_parse_error::invalid_file_format)
                            std::cout << "Invalid file format for input key!" << std::endl;
                        return 0;
                    }
                    auto settings = result.value();
                    mode_testing(settings.run_time_s, settings.threshold, settings.input, settings.input_columns, settings.delimiter);
                } else {
                    std::cout << "Invalid mode: " << mode << "given!" << std::endl;
                }

                return 0;
            }
        };
        

    }

    return 0;
}