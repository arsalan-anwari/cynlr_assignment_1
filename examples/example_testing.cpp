#include <iostream>
#include <iomanip>
#include <vector>

#include "data_pool.hpp"
#include "task.hpp"
#include "types.hpp"
#include "scheduler.hpp"

using namespace cynlr;

int main(){
    static constexpr usize application_run_time_s = 10u;

    static constexpr pool_type data_pool_mode = pool_type::producer_consumer_single_direction_static;

    using data_pool_type = data_pool<data_pool_mode, u8, 32u, 2u>;
    data_pool_type pool;
    
    std::vector<u8> buffer = {
        42, 17, 93, 8, 61, 29, 75, 50, 2, 84, 19, 34, 67, 21, 58, 90, 31, 73, 6, 88, 45, 12, 77, 99, 39, 64, 27, 81, 15, 55, 23, 53, 
        63, 53, 24, 11, 198, 167, 233, 154, 102, 200, 129, 174, 85, 214, 175, 250, 57, 206, 144, 187, 160, 223, 110, 199, 140, 231, 124, 189, 194, 244, 92, 210, 
        123, 220, 106, 184, 207, 221, 35, 191, 136, 247, 101, 242, 132, 228, 112, 215, 163, 232, 97, 216, 145, 229, 119, 197, 176, 252, 81, 237, 152, 201, 138, 249, 
        116, 208, 171, 239, 120, 211, 165, 238, 100, 213, 150, 236, 94, 188, 180, 241, 55, 190, 137, 225, 72, 222, 153, 217, 86, 209, 169, 226, 69, 212, 143, 230, 
        104, 178, 172, 240, 87, 192, 135, 218, 83, 219, 141, 234, 70, 203, 181, 248, 95, 202, 131, 235, 79, 193, 157, 246, 103, 233, 148, 243, 111, 185, 179, 245, 
        65, 204, 170, 224, 117, 218, 146, 227, 62, 212, 161, 254, 98, 206, 155, 232, 52, 202, 139, 255, 76, 222, 149, 223, 82, 181, 174, 255, 91, 201, 162, 246, 
        110, 211, 182, 249, 66, 195, 159, 242, 63, 220, 154, 253, 89, 207, 134, 233, 80, 213, 168, 239, 108, 214, 158, 243, 74, 208, 132, 252, 99, 193, 184, 221, 
        71, 197, 167, 226, 59, 204, 148, 245, 115, 224, 179, 251, 87, 194, 144, 234, 104, 198, 153, 232, 60, 187, 166, 219, 73, 221, 173, 236, 91, 209, 183, 228, 
        66, 193, 171, 249, 88, 202, 146, 255, 70, 196, 164, 253, 62, 199, 149, 250, 75, 214, 178, 240, 108, 205, 154, 224, 80, 212, 139, 237, 65, 197, 161, 247,
        92, 189, 181, 242, 52, 186, 137, 235, 104, 218, 156, 222, 59, 200, 172, 246, 69, 220, 133, 243, 85, 199, 187, 239, 111, 206, 165, 226, 56, 184, 150, 251, 
        77, 198, 136, 230, 100, 207, 162, 233, 58, 216, 151, 223, 94, 195, 175, 244, 83, 203, 140, 250, 67, 191, 183, 229, 103, 209, 174, 247, 71, 205, 156, 238, 
        93, 192, 162, 236, 78, 201, 157, 245, 105, 213, 147, 251, 55, 208, 177, 237, 102, 214, 141, 240, 86, 187, 153, 234, 60, 196, 184, 224, 88, 202, 164, 231, 
        52, 194, 171, 220, 76, 205, 147, 248, 111, 200, 179, 235, 66, 188, 140, 233, 85, 210, 158, 227, 68, 204, 136, 255, 53, 217, 177, 222, 79, 215, 143, 251, 
        62, 198, 169, 228, 70, 190, 185, 237, 107, 208, 166, 224, 57, 203, 149, 252, 97, 211, 138, 235, 89, 193, 179, 229, 64, 191, 173, 231, 106, 199, 165, 254, 
        81, 218, 187, 230, 63, 200, 142, 238, 90, 207, 171, 233, 66, 195, 168, 246, 95, 203, 160, 241, 74, 197, 156, 223, 61, 210, 181, 249, 113, 189, 151, 245, 
        157, 232, 102, 213, 163, 228, 75, 216, 141, 236, 51, 202, 186, 221, 99, 208, 147, 250, 86, 206, 148, 226, 68, 204, 159, 243, 109, 219, 182, 234, 67, 194, 
        138, 230, 92, 215, 173, 241, 55, 191, 154, 254, 64, 196, 137, 248, 81, 209, 170, 239, 112, 200, 146, 227, 60, 198, 163, 252, 73, 211, 151, 246, 69, 218, 
        159, 234, 108, 206, 140, 225, 52, 193, 176, 253, 99, 217, 161, 239, 86, 195, 157, 232, 77, 203, 143, 250, 63, 210, 166, 236, 90, 198, 171, 233, 55, 204, 
        149, 237, 108, 209, 177, 243, 71, 195, 147, 249, 104, 214, 140, 223, 61, 191, 160, 245, 93, 202, 174, 228, 72, 194, 153, 239, 95, 215, 185, 250, 58, 201, 
        146, 232, 79, 213, 167, 221, 64, 207, 141, 244, 102, 200, 188, 230, 54, 210, 182, 231, 81, 199, 158, 247, 75, 208, 153, 240, 112, 193, 144, 228, 70, 196, 
        97, 118, 145, 234, 243, 111
    };

    std::array<f32, 9> filter_weights = {0.00025177, 0.008666992, 0.078025818, 0.24130249, 0.343757629, 0.24130249, 0.078025818, 0.008666992, 0.000125885};
    
    const std::span<u8> tx_data(buffer);
    task_data_generator<task_mode::testing, data_pool_type> task1(pool, tx_data);
    task_filter_threshold<task_mode::testing, data_pool_type, f32, u8, 4u> task2(pool, filter_weights, 175u);

    scheduler sched;
    sched.add_task(task1);
    sched.add_task(task2);

    std::this_thread::sleep_for(std::chrono::seconds(application_run_time_s));

    sched.stop();

    std::this_thread::sleep_for(std::chrono::seconds(1));

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

    return 0;
}
