set(SAMPLE_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/actor.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/command.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/renderer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/texture-region.cpp)

add_library(ptcsampleengine SHARED ${SAMPLE_SOURCES})