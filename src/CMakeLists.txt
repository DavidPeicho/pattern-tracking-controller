list(APPEND PTC_SOURCES ptc.cpp ptc-impl.cpp input-processor.cpp)

find_package(OpenCV REQUIRED)

add_subdirectory(input)
add_subdirectory(processing)
add_subdirectory(utils)

add_library(ptc SHARED ${PTC_SOURCES})

target_link_libraries(ptc ${OpenCV_LIBS})
