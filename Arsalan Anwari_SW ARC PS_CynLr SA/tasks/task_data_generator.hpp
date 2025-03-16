#pragma once

#include <array>
#include <cmath>

#include "task_types.hpp"
#include "../data_pool.hpp"
#include "../logging.hpp"


constexpr usize roundUp(usize dividend, usize divisor) {
    return (dividend + divisor - 1) / divisor;
}

namespace cynlr{

template<
    task_mode Mode, 
    pool_type PoolType,
    std::integral DataType,
    usize TxBatchSize,
    usize RxBatchSize,  
    typename... Args
>
struct task_data_generator;

template<
    pool_type PoolType,
    typename DataType,
    usize TxBatchSize,
    usize RxBatchSize
>
struct task_data_generator<task_mode::testing, PoolType, DataType, TxBatchSize, RxBatchSize>{
    static constexpr task_mode type = task_mode::testing;
    using shared_data_pool = data_pool<PoolType, DataType, csize<RxBatchSize>>;

    task_data_generator(shared_data_pool& shared_pool, std::vector<DataType> tx_data):
        resource(shared_pool.resource),
        tx_done(shared_pool.tx_done_flag),
        tx_data(tx_data)
    {}

    auto task_settings() -> task_settings { return {"Data Generator [TestingMode]", 1000}; };

    std::expected<task_response, task_error> setup() {
        num_iterations = roundUp(tx_data.size(), TxBatchSize);
        Log(std::format("Data Generator -> setup() -> num_iterations = {}", num_iterations));
        return task_response::success;
    }
    
    std::expected<task_response, task_error> loop() {
		if (batch > num_iterations) return task_response::stop;
        
        for(usize i = 0; i < TxBatchSize; i++){
            idx = batch * TxBatchSize;
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

    shared_data_pool::buffer_type& resource;
    shared_data_pool::flag_type& tx_done;

    std::vector<DataType> tx_data;
 
    usize batch = 0;
    usize idx = 0;
    usize num_iterations = tx_data.size() / TxBatchSize;
    
};

} //namespace cynlr