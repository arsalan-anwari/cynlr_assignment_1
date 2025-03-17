# Include application source directory for headers
include_directories("${PROJECT_SOURCE_DIR}/app")

# FetchContent for Catch2
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(
    TESTS 
    test_algorithms.cpp
    test_traits.cpp
    test_args.cpp
    test_csv_reader.cpp
    test_data_pool.cpp
)

include(GoogleTest)

enable_testing()

foreach(TEST ${TESTS})
    get_filename_component(TEST_NAME ${TEST} NAME_WE)
    add_executable(${TEST_NAME} ${TEST})
    target_link_libraries(${TEST_NAME} GTest::gtest_main)

    gtest_discover_tests(${TEST_NAME})
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

endforeach()

