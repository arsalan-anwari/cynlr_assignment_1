#pragma once

#include <format>

#include "task_data_generator_base.hpp"
#include "../task_types.hpp"
#include "../../logging.hpp"
#include "../../algorithms/clamping.hpp"

namespace cynlr{

template<data_pool_like DataPoolType>
struct task_data_generator<task_mode::testing, DataPoolType>{
    static constexpr task_mode type = task_mode::testing;
    static constexpr usize tx_batch_size = DataPoolType::tx_batch_size;
    
    using DataType = DataPoolType::buffer_data_type;

    task_data_generator(DataPoolType& shared_pool, std::vector<DataType> tx_data):
        resource(shared_pool.resource),
        tx_done(shared_pool.tx_done_flag),
        tx_data(tx_data)
    {}

    auto task_settings() -> task_settings { return {"Data Generator [TestingMode]", settings::task_data_generator::DeadlineTaskNs}; };

    std::expected<task_response, task_error> setup() {
        num_iterations = clamp_top(tx_data.size(), tx_batch_size);
        Log(std::format("Data Generator -> setup() -> num_iterations = {}", num_iterations));
        return task_response::success;
    }
    
    std::expected<task_response, task_error> loop() {
		if (batch > num_iterations) return task_response::stop;
        
        for(usize i = 0; i < tx_batch_size; i++){
            idx = batch * tx_batch_size;
            if (idx + i >= tx_data.size()) return task_response::stop;
            resource.transmit(tx_data[idx + i]);
        }
        batch++;
        
        return task_response::success;
    }
    
    std::expected<task_response, task_error> stop() { 
        Log("Data Generator -> stop()");
        tx_done.store(true);
		return task_response::success; 
	}

private:

    DataPoolType::buffer_type& resource;
    DataPoolType::flag_type& tx_done;

    std::vector<DataType> tx_data;
 
    usize batch = 0;
    usize idx = 0;
    usize num_iterations;
    
};

} //namespace cynlr