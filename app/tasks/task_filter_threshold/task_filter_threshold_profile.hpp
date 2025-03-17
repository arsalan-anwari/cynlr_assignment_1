#pragma once

#include <cmath>
#include <array>
#include <chrono>

#include "task_filter_threshold_base.hpp"

#include "../task_types.hpp"
#include "../../settings.hpp"
#include "../../algorithms/copy.hpp"
#include "../../algorithms/filter.hpp"
#include "../../algorithms/threshold.hpp"

namespace cynlr{

    template<
        data_pool_like DataPoolType,
        std::floating_point FilterWeightDataType,
        threshold_data_like ThresholdDataType,
        usize FilterWindowRadius
    >
    struct task_filter_threshold<task_mode::profiling, DataPoolType, FilterWeightDataType, ThresholdDataType, FilterWindowRadius> {
        
        static constexpr task_mode type = task_mode::profiling;
        static constexpr usize swap_size = (FilterWindowRadius * 2);
        static constexpr usize window_size = swap_size + 1;
        static constexpr usize rx_batch_size = DataPoolType::rx_batch_size;
        
        using DataType = DataPoolType::buffer_data_type;

        task_filter_threshold(DataPoolType& shared_pool, std::array<FilterWeightDataType, swap_size + 1>& filter_weights, DataType threshold) :
            resource(shared_pool.resource),
            filter_weights(filter_weights),
            threshold(threshold),
            rx_durations(shared_pool.rx_durations)
        {
            thresholded_data.reserve(rx_batch_size);
            rx_durations.reserve(settings::ProfileBufferSize);
        }

        auto task_settings() -> task_settings { return {"Filter & Threshold [Debug]", 0}; };

        auto setup() -> std::expected<task_response, task_error> {
            Log("Filter & Threshold -> setup()");
            return task_response::success;
        }
        
        auto loop() -> std::expected<task_response, task_error> {
            if(resource.receive(value)){
                consumer_buffer[index++] = value;
                if (index == index_end) {   
                    auto start_time = std::chrono::high_resolution_clock::now();
                    
                    filter_window<DataType, FilterWeightDataType, window_size>(consumer_buffer, filter_weights, front_offset, back_offset);
                    threshold_copy<DataType, ThresholdDataType>(consumer_buffer, thresholded_data, threshold, front_offset, back_offset);
                    copy_tail_to_head<DataType, swap_size>(consumer_buffer, additional_back_offset_swap);

                    if(first_time){
                        index_end = buffer_size;
                        additional_back_offset_swap = 0; 
                        back_offset = front_offset;
                        first_time = false;
                    }
                    
                    index = swap_size;

                    auto end_time = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
                    if (duration > 0 && duration < settings::task_data_generator::DeadlineTaskNs) 
                        rx_durations.push_back(duration);

                    thresholded_data.clear();
                }
            }
            
            return task_response::yield;
        }
        
        auto stop() -> std::expected<task_response, task_error> { 
            Log("Filter & Threshold -> stop()");
            if (index > 0){ // There is still some data remaining.
                thresholded_data.clear();

                auto start_time = std::chrono::high_resolution_clock::now();

                std::span<DataType> tail_buffer = std::span<DataType>(consumer_buffer.begin(), index + front_offset); 
                std::fill(tail_buffer.end() - back_offset, tail_buffer.end(), static_cast<DataType>(0u));
                
                filter_window<DataType, FilterWeightDataType, window_size>(tail_buffer, filter_weights, front_offset, back_offset);
                threshold_copy<DataType, ThresholdDataType>(tail_buffer, thresholded_data, threshold, front_offset, back_offset);
                
                auto end_time = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
                if (duration > 0 && duration < settings::task_data_generator::DeadlineTaskNs) rx_durations.push_back(duration); 

            }
            return task_response::success; 
        }

    private:
    
        DataType value;
        DataType threshold;
        
        const std::array<FilterWeightDataType, window_size>& filter_weights;
        std::array<DataType, rx_batch_size + swap_size> consumer_buffer = {0};
        std::vector<ThresholdDataType> thresholded_data;

        bool first_time = true;
        size_t index = FilterWindowRadius;
        size_t front_offset = FilterWindowRadius;
        size_t back_offset = front_offset * 2; // // in first iteration ignore the last n elements
        size_t buffer_size = consumer_buffer.size();
    
        size_t index_end = buffer_size - front_offset; // in first iteration ignore the last n elements
        size_t additional_back_offset_swap = FilterWindowRadius;

        DataPoolType::buffer_type& resource;
        std::vector<time_ns>& rx_durations;
    };


} // namespace cynlr