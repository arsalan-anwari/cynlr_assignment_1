#include <gtest/gtest.h>

#include "traits/type_format.hpp"

namespace cynlr {

  TEST(TypeFormatTraitTest, HandlesI8) {
    EXPECT_EQ(type_format_trait::type_info<i8>::name(), "i8");
}

TEST(TypeFormatTraitTest, HandlesI16) {
    EXPECT_EQ(type_format_trait::type_info<i16>::name(), "i16");
}

TEST(TypeFormatTraitTest, HandlesI32) {
    EXPECT_EQ(type_format_trait::type_info<i32>::name(), "i32");
}

TEST(TypeFormatTraitTest, HandlesI64) {
    EXPECT_EQ(type_format_trait::type_info<i64>::name(), "i64");
}

TEST(TypeFormatTraitTest, HandlesU8) {
    EXPECT_EQ(type_format_trait::type_info<u8>::name(), "u8");
}

TEST(TypeFormatTraitTest, HandlesU16) {
    EXPECT_EQ(type_format_trait::type_info<u16>::name(), "u16");
}

TEST(TypeFormatTraitTest, HandlesU32) {
    EXPECT_EQ(type_format_trait::type_info<u32>::name(), "u32");
}

TEST(TypeFormatTraitTest, HandlesU64) {
    EXPECT_EQ(type_format_trait::type_info<u64>::name(), "u64");
}

TEST(TypeFormatTraitTest, HandlesCsize) {
    EXPECT_EQ(type_format_trait::type_info<csize<10>>::name(), "csize");
}

TEST(TypeFormatTraitTest, HandlesF32) {
    EXPECT_EQ(type_format_trait::type_info<f32>::name(), "f32");
}

TEST(TypeFormatTraitTest, HandlesF64) {
    EXPECT_EQ(type_format_trait::type_info<f64>::name(), "f64");
}

TEST(TypeFormatTraitTest, HandlesStr) {
    EXPECT_EQ(type_format_trait::type_info<str>::name(), "str");
}

TEST(TypeFormatTraitTest, HandlesCstr) {
    EXPECT_EQ(type_format_trait::type_info<cstr>::name(), "cstr");
}

TEST(TypeFormatTraitTest, HandlesBool) {
    EXPECT_EQ(type_format_trait::type_info<bool>::name(), "bool");
}

TEST(TypeFormatTraitTest, HandlesUnknownType) {
    struct UnknownType {};
    EXPECT_EQ(type_format_trait::type_info<UnknownType>::name(), "any");
}
    
    

} // namespace cynlr

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}