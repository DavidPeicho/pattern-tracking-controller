cmake_minimum_required(VERSION 3.0)

project(PatternTrackingController)

include(CMakeToolsHelpers OPTIONAL)

# C++14 and extra flags
list(APPEND CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wextra -std=c++1y -O0")

include_directories(
        ${CMAKE_SOURCE_DIR}/src
        ${OpenCV_INCLUDE_DIRS}
)

# Adds source directory
add_subdirectory(src)
add_subdirectory(samples)
