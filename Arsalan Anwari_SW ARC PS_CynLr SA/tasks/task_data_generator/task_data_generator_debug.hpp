#pragma once

#include <array>
#include <format>

#include "task_data_generator_base.hpp"
#include "../task_types.hpp"
#include "../../logging.hpp"
#include "../../algorithms/random.hpp"
#include "../../algorithms/formatting.hpp"
#include "../../data_pool.hpp"

namespace cynlr{

template<data_pool_like DataPoolType>
struct task_data_generator<task_mode::debug, DataPoolType>{
    static constexpr task_mode type = task_mode::debug;
    static constexpr usize tx_batch_size = DataPoolType::tx_batch_size;
    
    using DataType = DataPoolType::buffer_data_type;

    task_data_generator(DataPoolType& shared_pool):
        resource(shared_pool.resource)
    {}

    auto task_settings() -> task_settings { return {"Data Generator [Debug]", settings::task_data_generator::DeadlineTaskDebugNs}; };

    std::expected<task_response, task_error> setup() {
        Log(std::format("Data Generator -> setup()"));
        return task_response::success;
    }
    
    std::expected<task_response, task_error> loop() {
        for(auto& e : tx_data){
            e = rng();
        }

        for(auto e : tx_data){
            resource.transmit(e);
        }
        
        Log(std::format("TX Out: ({})", join_str<DataType>(tx_data)));

        return task_response::success;
    }
    
    std::expected<task_response, task_error> stop() { 
        Log("Data Generator -> stop()");
		return task_response::success; 
	}

private:
    DataPoolType::buffer_type& resource;
    std::array<DataType, tx_batch_size> tx_data;
    random<random_engine::xor_shift, DataType> rng;
};


} //namespace cynlr