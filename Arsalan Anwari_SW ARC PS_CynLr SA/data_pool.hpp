#pragma once

#include <atomic>
#include "shared_resource.hpp"
#include "tasks/task_types.hpp"


namespace cynlr {

enum class pool_type : u8 {
    producer_consumer_single_direction_static,
    producer_consumer_bidirectional
};

template<pool_type T, typename... Args>
struct data_pool;

template<typename DataType, usize DataSize> 
struct data_pool<
    pool_type::producer_consumer_single_direction_static, 
    DataType, 
    csize<DataSize>
>{
    static constexpr pool_type type = pool_type::producer_consumer_single_direction_static;

    using buffer_type = shared_resource<resource_type::static_spsc_queue, DataType, csize<DataSize>>;
    using flag_type = std::atomic<bool>;
    
    buffer_type resource;
    flag_type tx_done_flag{false};

};



};
