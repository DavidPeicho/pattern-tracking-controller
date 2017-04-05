cmake_minimum_required(VERSION 3.0)

project(PatternTrackingController)

# C++14 and extra flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wextra -std=c++1y")

# Set library output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/bin/samples)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/lib)

# Add source directory
add_subdirectory(src)
add_subdirectory(samples)
