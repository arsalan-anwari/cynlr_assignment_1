#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <numeric> // for accumulate

#include "data_pool.hpp"
#include "task.hpp"
#include "types.hpp"
#include "scheduler.hpp"


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

using namespace cynlr;

int main(){
 
    static constexpr pool_type data_pool_mode = pool_type::producer_consumer_profiling_static;
    static constexpr usize application_run_time_s = 10u;
    
    using data_pool_type = data_pool<data_pool_mode, u8, 32u, 2u>;
    data_pool_type pool;

    std::array<f32, 9> filter_weights = {0.00025177, 0.008666992, 0.078025818, 0.24130249, 0.343757629, 0.24130249, 0.078025818, 0.008666992, 0.000125885};
    
    task_data_generator<task_mode::profiling, data_pool_type> task1(pool);
    task_filter_threshold<task_mode::profiling, data_pool_type, f32, u8, 4u> task2(pool, filter_weights, 175u);

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

    return 0;
}
