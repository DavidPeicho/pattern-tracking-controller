set(PTC_SOURCES ptc.cpp ptc-impl.cpp
        )

find_package(OpenCV REQUIRED)

add_subdirectory(input)
add_subdirectory(processing)
add_subdirectory(surf)
add_subdirectory(utils)

add_library(ptc SHARED ${PTC_SOURCES})

target_link_libraries(ptc ${OpenCV_LIBS})

# Copies assets to build dir
add_custom_command(TARGET ptc POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/samples/assets/ ${CMAKE_BINARY_DIR}/samples/assets)
