cmake_minimum_required(VERSION 3.10)
project(IDR_Analysis VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

# Find Eigen - use header-only approach
include_directories("/tmp/eigen-3.4.0")

# Try to find system Eigen3 as well
find_package(Eigen3 QUIET)

# Main executable
add_executable(idr_analysis 
    src/main.cpp
    src/optimization.cpp
    src/utility.cpp
)

target_include_directories(idr_analysis PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
if(Eigen3_FOUND)
    target_link_libraries(idr_analysis PUBLIC Eigen3::Eigen)
endif()

# Output to bin directory
set_target_properties(idr_analysis PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Enable testing
enable_testing()

# Add test executable if tests exist
file(GLOB TEST_SOURCES "tests/*.cpp")
if(TEST_SOURCES)
    add_executable(run_tests ${TEST_SOURCES}
        src/optimization.cpp
        src/utility.cpp
    )
    target_include_directories(run_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    if(Eigen3_FOUND)
        target_link_libraries(run_tests PUBLIC Eigen3::Eigen)
    endif()
    
    add_test(NAME IDRTests COMMAND run_tests)
    set_target_properties(run_tests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
endif()

# Print build info
message(STATUS "")
message(STATUS "IDR Analysis - C++ Project")
message(STATUS "================================")
message(STATUS "C++ Standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "")
