#pragma once

#include <chrono>
#include <array>
#include <format>

#include "task_data_generator_base.hpp"
#include "../task_types.hpp"
#include "../../logging.hpp"
#include "../../algorithms/random.hpp"
#include "../../data_pool.hpp"

namespace cynlr{

template<data_pool_like DataPoolType>
struct task_data_generator<task_mode::profiling, DataPoolType>{
    static constexpr task_mode type = task_mode::profiling;
    static constexpr usize tx_batch_size = DataPoolType::tx_batch_size;
    static constexpr usize rx_batch_size = DataPoolType::rx_batch_size;
    
    using DataType = DataPoolType::buffer_data_type;

    task_data_generator(DataPoolType& shared_pool):
        resource(shared_pool.resource),
        tx_durations(shared_pool.tx_durations)
    {
        tx_durations.reserve(rx_batch_size + settings::ProfileBufferSize);
    }

    auto task_settings() -> task_settings { return {"Data Generator [Profiling]", settings::task_data_generator::DeadlineTaskNs}; };

    auto setup() -> std::expected<task_response, task_error> {
        Log(std::format("Data Generator -> setup()"));
        return task_response::success;
    }
    
    auto loop() -> std::expected<task_response, task_error> {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for(auto& e : tx_data){
            e = rng();
        }

        for(auto e : tx_data){
            resource.transmit(e);
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
        
        if (duration > 0 && duration < settings::task_data_generator::DeadlineTaskNs) 
            tx_durations.push_back(duration); 

        return task_response::success;
    }
    
    auto stop() -> std::expected<task_response, task_error> { 
        Log("Data Generator -> stop()");
		return task_response::success; 
	}

private:
    DataPoolType::buffer_type& resource;
    std::vector<time_ns>& tx_durations;

    std::array<DataType, tx_batch_size> tx_data;
    random<random_engine::xor_shift, DataType> rng;
};


} //namespace cynlr