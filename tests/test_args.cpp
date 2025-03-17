#include <gtest/gtest.h>
#include "args.hpp"

namespace cynlr {

TEST(StrToArgKeyTest, HandlesHelp) {
    EXPECT_EQ(str_to_arg_key("help"), arg_key::help);
}

TEST(StrToArgKeyTest, HandlesMode) {
    EXPECT_EQ(str_to_arg_key("mode"), arg_key::mode);
}

TEST(StrToArgKeyTest, HandlesInput) {
    EXPECT_EQ(str_to_arg_key("input"), arg_key::input);
}

TEST(StrToArgKeyTest, HandlesInputColumns) {
    EXPECT_EQ(str_to_arg_key("input_columns"), arg_key::input_columns);
}

TEST(StrToArgKeyTest, HandlesDelimiter) {
    EXPECT_EQ(str_to_arg_key("delimiter"), arg_key::delimiter);
}

TEST(StrToArgKeyTest, HandlesRunTimeS) {
    EXPECT_EQ(str_to_arg_key("run_time_s"), arg_key::run_time_s);
}

TEST(StrToArgKeyTest, HandlesThreshold) {
    EXPECT_EQ(str_to_arg_key("threshold"), arg_key::threshold);
}

TEST(StrToArgKeyTest, HandlesInvalid) {
    EXPECT_EQ(str_to_arg_key("unknown_key"), arg_key::invalid);
}

} // namespace cynlr

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}