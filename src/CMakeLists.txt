set(PTC_SOURCES ptc.cpp ptc-impl.cpp)

find_package(OpenCV REQUIRED)

add_subdirectory(input)

add_library(ptc SHARED ${PTC_SOURCES})

target_link_libraries(ptc ${OpenCV_LIBS})
