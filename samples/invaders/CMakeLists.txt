set(SAMPLE_LIST ${SAMPLE_LIST} invaders-sample PARENT_SCOPE)

# Adds 'invaders-sample` target
add_executable(invaders-sample moving-actor.cpp bullet-actor.cpp move-command.cpp ia.cpp world main.cpp)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/samples/invaders/cmake_modules" ${CMAKE_MODULE_PATH})
find_package(SFML 2 REQUIRED system window graphics network audio)
if(SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
    target_link_libraries(invaders-sample ptc ptcsampleengine ${SFML_LIBRARIES})
endif()