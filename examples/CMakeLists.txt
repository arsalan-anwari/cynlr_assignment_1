include_directories("${PROJECT_SOURCE_DIR}/app")

set(
    EXAMPLES 
    example_csv_reader.cpp 
    example_debug.cpp 
    example_profile.cpp 
    example_schedular.cpp 
    example_testing.cpp
)

foreach(EXAMPLE ${EXAMPLES})
    get_filename_component(EXAMPLE_NAME ${EXAMPLE} NAME_WE)
    add_executable(${EXAMPLE_NAME} ${EXAMPLE})
endforeach()

