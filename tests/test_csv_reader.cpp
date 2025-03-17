#include <gtest/gtest.h>
#include <fstream>
#include "csv_reader.hpp"

using namespace cynlr;

class CsvReaderTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a temporary valid CSV file for testing
        std::ofstream valid_file("valid.csv");
        valid_file << "1,2,3\n4,5,6\n";
        valid_file.close();

        // Create a temporary invalid CSV file for testing
        std::ofstream invalid_format_file("invalid_format.csv");
        invalid_format_file << "1,2,abc\n4,5,6\n";
        invalid_format_file.close();

        // Create a temporary out of range CSV file for testing
        std::ofstream out_of_range_file("out_of_range.csv");
        out_of_range_file << "1,2,999999999999999999999999999999\n4,5,6\n";
        out_of_range_file.close();
    }

    void TearDown() override {
        // Remove temporary files after testing
        std::remove("valid.csv");
        std::remove("invalid_format.csv");
        std::remove("out_of_range.csv");
    }
};

TEST_F(CsvReaderTest, ValidCsvFile) {
    csv_reader<int> reader("valid.csv", 3);
    auto result = reader.parse();
    ASSERT_TRUE(result.has_value());
    auto data = result.value();
    EXPECT_EQ(data.size(), 6);
    EXPECT_EQ(data[0], 1);
    EXPECT_EQ(data[1], 2);
    EXPECT_EQ(data[2], 3);
    EXPECT_EQ(data[3], 4);
    EXPECT_EQ(data[4], 5);
    EXPECT_EQ(data[5], 6);
}

TEST_F(CsvReaderTest, InvalidFileName) {
    csv_reader<int> reader("non_existent.csv", 3);
    auto result = reader.parse();
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), csv_reader_error::invalid_file_name);
}

TEST_F(CsvReaderTest, InvalidDataFormat) {
    csv_reader<int> reader("invalid_format.csv", 3);
    auto result = reader.parse();
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), csv_reader_error::invalid_data_format);
}

TEST_F(CsvReaderTest, DataOutOfRange) {
    csv_reader<int> reader("out_of_range.csv", 3);
    auto result = reader.parse();
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), csv_reader_error::invalid_data_format);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}