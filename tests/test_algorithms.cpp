#include <gtest/gtest.h>

#include "algorithm.hpp"

namespace cynlr {

TEST(ClampTopTest, HandlesZeroDividend) {
    EXPECT_EQ(clamp_top(0, 1), 0);
}

TEST(ClampTopTest, HandlesZeroDivisor) {
    EXPECT_EQ(clamp_top(1, 0), 0);
}

TEST(ClampTopTest, HandlesExactDivision) {
    EXPECT_EQ(clamp_top(10, 2), 5);
}

TEST(ClampTopTest, HandlesNonExactDivision) {
    EXPECT_EQ(clamp_top(10, 3), 4);
}

TEST(ClampTopTest, HandlesLargeNumbers) {
    EXPECT_EQ(clamp_top(1000000, 3), 333334);
}

TEST(CopyTailToHeadTest, HandlesIntegralType) {
    std::vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::span<int> data_span(data);
    copy_tail_to_head<int, 3>(data_span, 0);
    EXPECT_EQ(data[0], 8);
    EXPECT_EQ(data[1], 9);
    EXPECT_EQ(data[2], 10);
}

TEST(CopyTailToHeadTest, HandlesAdditionalBackOffset) {
    std::vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::span<int> data_span(data);
    copy_tail_to_head<int, 3>(data_span, 2);
    EXPECT_EQ(data[0], 6);
    EXPECT_EQ(data[1], 7);
    EXPECT_EQ(data[2], 8);
}

TEST(CopyTailToHeadTest, HandlesSpecializationForU8) {
    std::vector<u8> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    std::span<u8> data_span(data);
    copy_tail_to_head<u8, 8u>(data_span, 0);
    EXPECT_EQ(data[0], 9);
    EXPECT_EQ(data[1], 10);
    EXPECT_EQ(data[2], 11);
    EXPECT_EQ(data[3], 12);
    EXPECT_EQ(data[4], 13);
    EXPECT_EQ(data[5], 14);
    EXPECT_EQ(data[6], 15);
    EXPECT_EQ(data[7], 16);
}

TEST(FilterWindowTest, HandlesWindowSize9) {
    std::vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
    std::vector<int> filter_weights = {1, 1, 1, 1, 1, 1, 1, 1, 1};
    std::span<int> data_span(data);
    std::span<const int> filter_weights_span(filter_weights);
    filter_window<int, int, 9>(data_span, filter_weights_span, 4, 0);
    EXPECT_EQ(data[4], 45);
}

TEST(FilterWindowTest, HandlesOutputVector) {
    std::vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
    std::vector<int> filter_weights = {1, 1, 1, 1, 1, 1, 1, 1, 1};
    std::vector<int> out;
    std::span<int> data_span(data);
    std::span<const int> filter_weights_span(filter_weights);
    filter_window<int, int, 9>(data_span, out, filter_weights_span, 4, 0);
    EXPECT_EQ(data[4], 45);
    EXPECT_EQ(out[0], 45);
    EXPECT_EQ(out[0], data[4]);
}

TEST(JoinStrTest, HandlesEmptyInput) {
    std::vector<int> data = {};
    std::span<const int> data_span(data);
    EXPECT_EQ(join_str(data_span), "");
}

TEST(JoinStrTest, HandlesSingleElement) {
    std::vector<int> data = {42};
    std::span<const int> data_span(data);
    EXPECT_EQ(join_str(data_span), "42");
}

TEST(JoinStrTest, HandlesMultipleElements) {
    std::vector<int> data = {1, 2, 3, 4, 5};
    std::span<const int> data_span(data);
    EXPECT_EQ(join_str(data_span), "1,2,3,4,5");
}

TEST(JoinStrTest, HandlesCustomSeparator) {
    std::vector<int> data = {1, 2, 3, 4, 5};
    std::span<const int> data_span(data);
    EXPECT_EQ(join_str(data_span, ';'), "1;2;3;4;5");
}

TEST(RandomXorShiftTest, GeneratesConsistentU64) {
    random<random_engine::xor_shift, u64> rng1;
    random<random_engine::xor_shift, u64> rng2;
    EXPECT_NE(rng1(), rng2());
}

TEST(RandomXorShiftTest, GeneratesConsistentU8) {
    random<random_engine::xor_shift, u8> rng1;
    random<random_engine::xor_shift, u8> rng2;
    EXPECT_NE(rng1(), rng2());
}

TEST(RandomXorShiftTest, GeneratesDifferentValues) {
    random<random_engine::xor_shift, u64> rng;
    auto value1 = rng();
    auto value2 = rng();
    EXPECT_NE(value1, value2);
}

TEST(RandomXorShiftTest, GeneratesDifferentU8Values) {
    random<random_engine::xor_shift, u8> rng;
    auto value1 = rng();
    auto value2 = rng();
    EXPECT_NE(value1, value2);
}

TEST(ThresholdCopyTest, HandlesNoOffset) {
    std::vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::vector<u8> thresholded_data;
    std::span<const int> data_span(data);
    threshold_copy(data_span, thresholded_data, 5, 0, 0);
    std::vector<u8> expected = {0, 0, 0, 0, 1, 1, 1, 1, 1, 1};
    EXPECT_EQ(thresholded_data, expected);
}

TEST(ThresholdCopyTest, HandlesFrontOffset) {
    std::vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::vector<u8> thresholded_data;
    std::span<const int> data_span(data);
    threshold_copy(data_span, thresholded_data, 5, 2, 0);
    std::vector<u8> expected = {0, 0, 1, 1, 1, 1, 1, 1};
    EXPECT_EQ(thresholded_data, expected);
}

TEST(ThresholdCopyTest, HandlesBackOffset) {
    std::vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::vector<u8> thresholded_data;
    std::span<const int> data_span(data);
    threshold_copy(data_span, thresholded_data, 5, 0, 2);
    std::vector<u8> expected = {0, 0, 0, 0, 1, 1, 1, 1};
    EXPECT_EQ(thresholded_data, expected);
}

TEST(ThresholdCopyTest, HandlesBothOffsets) {
    std::vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::vector<u8> thresholded_data;
    std::span<const int> data_span(data);
    threshold_copy(data_span, thresholded_data, 5, 2, 2);
    std::vector<u8> expected = {0, 0, 1, 1, 1, 1};
    EXPECT_EQ(thresholded_data, expected);
}

} // namespace cynlr

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}