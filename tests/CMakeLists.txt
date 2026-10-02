cmake_minimum_required(VERSION 3.20)
project(SortingBenchmarkerTests LANGUAGES CXX)

add_executable(test_sorting
    test_sorting_correctness.cpp
)

target_link_libraries(test_sorting PRIVATE sorting_core)

add_test(NAME sorting_correctness COMMAND test_sorting)

# Ensure LLVM/MinGW runtime DLLs are findable when CTest runs
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    get_filename_component(COMPILER_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
    set_tests_properties(sorting_correctness PROPERTIES
        ENVIRONMENT "PATH=${COMPILER_BIN_DIR};$ENV{PATH}"
    )
endif()