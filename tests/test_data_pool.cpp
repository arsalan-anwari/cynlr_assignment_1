#include <gtest/gtest.h>
#include "data_pool.hpp"
#include "shared_resource.hpp" // Include shared_resource header
#include "types.hpp" // Include types header

using namespace cynlr;

TEST(DataPoolTest, ProducerConsumerSingleDirectionStaticInt) {
    data_pool<pool_type::producer_consumer_single_direction_static, int, 2u, 32u> pool;
    EXPECT_EQ(pool.type, pool_type::producer_consumer_single_direction_static);
    EXPECT_EQ(pool.rx_batch_size, 2u);
    EXPECT_EQ(pool.tx_batch_size, 32u);
    EXPECT_FALSE(pool.tx_done_flag.load());
}

TEST(DataPoolTest, ProducerConsumerSingleDirectionStaticFloat) {
    data_pool<pool_type::producer_consumer_single_direction_static, float, 2u, 32u> pool;
    EXPECT_EQ(pool.type, pool_type::producer_consumer_single_direction_static);
    EXPECT_EQ(pool.rx_batch_size, 2u);
    EXPECT_EQ(pool.tx_batch_size, 32u);
    EXPECT_FALSE(pool.tx_done_flag.load());
}

TEST(DataPoolTest, ProducerConsumerBidirectionalStaticInt) {
    data_pool<pool_type::producer_consumer_bidirectional_static, int, 2u, 32u> pool;
    EXPECT_EQ(pool.type, pool_type::producer_consumer_bidirectional_static);
    EXPECT_EQ(pool.rx_batch_size, 2u);
    EXPECT_EQ(pool.tx_batch_size, 32u);
    EXPECT_FALSE(pool.tx_done_flag.load());
    EXPECT_FALSE(pool.rx_done_flag.load());
}

TEST(DataPoolTest, ProducerConsumerBidirectionalStaticFloat) {
    data_pool<pool_type::producer_consumer_bidirectional_static, float, 2u, 32u> pool;
    EXPECT_EQ(pool.type, pool_type::producer_consumer_bidirectional_static);
    EXPECT_EQ(pool.rx_batch_size, 2u);
    EXPECT_EQ(pool.tx_batch_size, 32u);
    EXPECT_FALSE(pool.tx_done_flag.load());
    EXPECT_FALSE(pool.rx_done_flag.load());
}

TEST(DataPoolTest, ProducerConsumerDebugStaticInt) {
    data_pool<pool_type::producer_consumer_debug_static, int, 2u, 32u> pool;
    EXPECT_EQ(pool.type, pool_type::producer_consumer_debug_static);
    EXPECT_EQ(pool.rx_batch_size, 2u);
    EXPECT_EQ(pool.tx_batch_size, 32u);
}

TEST(DataPoolTest, ProducerConsumerDebugStaticFloat) {
    data_pool<pool_type::producer_consumer_debug_static, float, 2u, 32u> pool;
    EXPECT_EQ(pool.type, pool_type::producer_consumer_debug_static);
    EXPECT_EQ(pool.rx_batch_size, 2u);
    EXPECT_EQ(pool.tx_batch_size, 32u);
}

TEST(DataPoolTest, ProducerConsumerProfilingStaticInt) {
    data_pool<pool_type::producer_consumer_profiling_static, int, 2u, 32u> pool;
    EXPECT_EQ(pool.type, pool_type::producer_consumer_profiling_static);
    EXPECT_EQ(pool.rx_batch_size, 2u);
    EXPECT_EQ(pool.tx_batch_size, 32u);
    EXPECT_TRUE(pool.tx_durations.empty());
    EXPECT_TRUE(pool.rx_durations.empty());
}

TEST(DataPoolTest, ProducerConsumerProfilingStaticFloat) {
    data_pool<pool_type::producer_consumer_profiling_static, float, 2u, 32u> pool;
    EXPECT_EQ(pool.type, pool_type::producer_consumer_profiling_static);
    EXPECT_EQ(pool.rx_batch_size, 2u);
    EXPECT_EQ(pool.tx_batch_size, 32u);
    EXPECT_TRUE(pool.tx_durations.empty());
    EXPECT_TRUE(pool.rx_durations.empty());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}