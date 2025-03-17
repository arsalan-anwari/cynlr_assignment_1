#pragma once

#include <vector>
#include <atomic>

#include "settings.hpp"
#include "shared_resource.hpp"
#include "tasks/task_types.hpp"


namespace cynlr {

enum class pool_type : u8 {
    producer_consumer_single_direction_static,
    producer_consumer_bidirectional_static,
    producer_consumer_debug_static,
    producer_consumer_profiling_static
};

template<typename T>
concept data_pool_like = requires {
    { T::type } -> std::convertible_to<pool_type>;
    { T::rx_batch_size } -> std::convertible_to<usize>;
    { T::tx_batch_size } -> std::convertible_to<usize>;
    typename T::buffer_type;
    typename T::buffer_data_type;
    typename T::flag_type;
};

template<pool_type T, typename DataType, usize RxBatchSize, usize TxBatchSize>
struct data_pool;

template<typename DataType, usize RxBatchSize, usize TxBatchSize> 
struct data_pool<
    pool_type::producer_consumer_single_direction_static, 
    DataType, 
    RxBatchSize, 
    TxBatchSize
>{
    static constexpr pool_type type = pool_type::producer_consumer_single_direction_static;
    static constexpr usize rx_batch_size = RxBatchSize;
    static constexpr usize tx_batch_size = TxBatchSize;
    using buffer_type = shared_resource<resource_type::static_spsc_queue, DataType, csize<RxBatchSize>>;
    using buffer_data_type = DataType;
    using flag_type = std::atomic<bool>;
    
    buffer_type resource;
    flag_type tx_done_flag{false};

};

template<typename DataType, usize RxBatchSize, usize TxBatchSize> 
struct data_pool<
    pool_type::producer_consumer_bidirectional_static, 
    DataType, 
    RxBatchSize, 
    TxBatchSize
>{
    static constexpr pool_type type = pool_type::producer_consumer_bidirectional_static;
    static constexpr usize rx_batch_size = RxBatchSize;
    static constexpr usize tx_batch_size = TxBatchSize;
    using buffer_type = shared_resource<resource_type::static_spsc_queue, DataType, csize<RxBatchSize>>;
    using buffer_data_type = DataType;
    using flag_type = std::atomic<bool>;
    
    buffer_type resource;
    flag_type tx_done_flag{false};
    flag_type rx_done_flag{false};

};

template<typename DataType, usize RxBatchSize, usize TxBatchSize> 
struct data_pool<
    pool_type::producer_consumer_debug_static, 
    DataType, 
    RxBatchSize, 
    TxBatchSize
>{
    static constexpr pool_type type = pool_type::producer_consumer_debug_static;
    static constexpr usize rx_batch_size = RxBatchSize;
    static constexpr usize tx_batch_size = TxBatchSize;
    using buffer_type = shared_resource<resource_type::static_spsc_queue, DataType, csize<RxBatchSize>>;
    using buffer_data_type = DataType;
    using flag_type = std::atomic<bool>;
    
    buffer_type resource;

};

template<typename DataType, usize RxBatchSize, usize TxBatchSize> 
struct data_pool<
    pool_type::producer_consumer_profiling_static, 
    DataType, 
    RxBatchSize, 
    TxBatchSize
>{
    static constexpr pool_type type = pool_type::producer_consumer_profiling_static;
    static constexpr usize rx_batch_size = RxBatchSize;
    static constexpr usize tx_batch_size = TxBatchSize;
    using buffer_type = shared_resource<resource_type::static_spsc_queue, DataType, csize<RxBatchSize>>;
    using buffer_data_type = DataType;
    using flag_type = std::atomic<bool>;
    
    buffer_type resource;
    
    std::vector<time_ns> tx_durations;
    std::vector<time_ns> rx_durations;

};



};
