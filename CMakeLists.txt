cmake_minimum_required(VERSION 3.20)
project(SortingBenchmarker VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler warnings
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Build types
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_library(sorting_core INTERFACE)
target_include_directories(sorting_core INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(sorting_core INTERFACE cxx_std_20)

# Main executable
add_executable(sorting_benchmarker
    src/main.cpp
)
target_link_libraries(sorting_benchmarker PRIVATE sorting_core)

# Tests
enable_testing()
add_subdirectory(tests)

# Install targets (optional)
install(TARGETS sorting_benchmarker DESTINATION bin)