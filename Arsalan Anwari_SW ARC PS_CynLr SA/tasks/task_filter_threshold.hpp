#pragma once

#include <cmath>
#include <array>
#include <concepts>
#include <type_traits>

#include "settings.hpp"
#include "task_types.hpp"
#include "../data_pool.hpp"
#include "../algorithms/copy.hpp"

namespace cynlr{

    enum class threshold_data : u8 {
        corrupted = 0u,
        valid = 1u,
        ignored = 2u,
        _size = 3u,
    };

    template<typename T>
    concept threshold_data_like = std::unsigned_integral<T> || std::is_same_v<T, threshold_data>;

    template<
        task_mode Mode,
        pool_type PoolType,
        std::unsigned_integral PoolDataType,
        std::floating_point FilterWeightDataType,
        threshold_data_like ThresholdDataType,
        PoolDataType ThresholdValue,  
        usize RxBatchSize,
        usize FilterWindowRadius,
        typename... Args
    >
    struct task_filter_threshold{
        
        static constexpr task_mode type = task_mode::realtime;
        static constexpr usize swap_size = (FilterWindowRadius * 2);
        using shared_data_pool = data_pool<PoolType, PoolDataType, csize<RxBatchSize>>;

        task_filter_threshold(shared_data_pool& shared_pool, std::array<FilterWeightDataType, swap_size + 1>& filter_weights):
            resource(shared_pool.resource),
            tx_done(shared_pool.tx_done_flag),
            filter_weights(filter_weights)
        {
            thresholded_data.reserve(RxBatchSize + settings::task_filter_threshold::MinThresholdBufferSize);
        }

        auto task_settings() -> task_settings { return {"Filter & Threshold [RealTimeMode]", 0}; };

        auto setup() -> std::expected<task_response, task_error> {
            Log("Filter & Threshold -> setup()");
            return task_response::success;
        }
        
        auto loop() -> std::expected<task_response, task_error> {
            if(tx_done.load(std::memory_order_acquire)) return task_response::stop;
            if(resource.receive(value)){
                consumer_buffer[index++] = value;
                if (index == index_end) {
                    filter_step(consumer_buffer, front_offset, back_offset);
                    threshold_step(consumer_buffer, front_offset, back_offset);
                    copy_tail_to_head<PoolDataType, swap_size>(consumer_buffer, additional_back_offset_swap);
    
                    if(first_time){
                        index_end = buffer_size;
                        additional_back_offset_swap = 0; 
                        back_offset = front_offset;
                        first_time = false;
                    }
                    
                    index = swap_size;
                }
            }
            
            return task_response::yield;
        }
        
        auto stop() -> std::expected<task_response, task_error> { 
            Log("Filter & Threshold -> stop()");
            if (index > 0){ // There is still some data remaining.
                std::span<PoolDataType> tail_buffer = std::span<PoolDataType>(consumer_buffer.begin(), index + front_offset); 
                std::fill(tail_buffer.end() - back_offset, tail_buffer.end(), static_cast<PoolDataType>(0u));
        
                filter_step(tail_buffer, front_offset, back_offset);
                threshold_step(tail_buffer, front_offset, back_offset);
            }
            return task_response::success; 
        }

        auto get_threshold_data() -> std::span<const ThresholdDataType> {
            return thresholded_data;
        }

        auto get_filtered_data() -> std::span<const PoolDataType> {
            return filtered_data;
        }



    private:
    
        PoolDataType value;
        bool first_time = true;
        size_t index = FilterWindowRadius;
        size_t front_offset = FilterWindowRadius;
        size_t back_offset = front_offset * 2; // // in first iteration ignore the last n elements
        size_t buffer_size = consumer_buffer.size();
    
        size_t index_end = buffer_size - front_offset; // in first iteration ignore the last n elements
        size_t additional_back_offset_swap = FilterWindowRadius;
        
        const std::array<FilterWeightDataType, swap_size + 1>& filter_weights;
        std::array<PoolDataType, RxBatchSize + swap_size> consumer_buffer = {0};
        
        std::vector<ThresholdDataType> thresholded_data;
        std::vector<PoolDataType> filtered_data;

        shared_data_pool::buffer_type& resource;
        shared_data_pool::flag_type& tx_done;

        void filter_step(std::span<PoolDataType> data, usize front_offset, usize back_offset){
            PoolDataType sum;
            usize iter_size = data.size() - front_offset - back_offset;
            
            for(usize i = front_offset; i < data.size() - back_offset; i++){
                sum = 
                    static_cast<PoolDataType>(
                        std::floor(
                            (data[i - FilterWindowRadius] * filter_weights[0]) 
                            + (data[i - FilterWindowRadius + 1] * filter_weights[1])
                            + (data[i - FilterWindowRadius + 2] * filter_weights[2])
                            + (data[i - FilterWindowRadius + 3] * filter_weights[3])
                            + (data[i - FilterWindowRadius + 4] * filter_weights[4])
                            + (data[i - FilterWindowRadius + 5] * filter_weights[5])
                            + (data[i - FilterWindowRadius + 6] * filter_weights[6])
                            + (data[i - FilterWindowRadius + 7] * filter_weights[7])
                            + (data[i - FilterWindowRadius + 8] * filter_weights[8])
                        )
                    );
                
                data[i] = sum;
                filtered_data.push_back(sum);
            }
        };
        
        void threshold_step(std::span<const PoolDataType> in, usize front_offset, usize back_offset){
            usize iter_size = in.size() - front_offset - back_offset;
            usize end_iter = front_offset + iter_size;
            for(usize i = front_offset; i < end_iter; i++){
                thresholded_data.push_back(
                    in[i] >= ThresholdValue ? 
                    static_cast<ThresholdDataType>(1u) : 
                    static_cast<ThresholdDataType>(0u)
                );
            }
        }

        void print_buffer(std::span<const PoolDataType> data, std::string_view name){
            std::cout << std::setw(20) << name << " [" << data.size() << "]: \t";
            for(const PoolDataType e : data) 
                std::cout << std::setw(3) << static_cast<int>(e) << " ";
            std::cout << std::endl;
        }

    };











} // namespace cynlr