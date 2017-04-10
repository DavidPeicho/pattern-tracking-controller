set(SAMPLE_LIST ${SAMPLE_LIST} process PARENT_SCOPE)

# Adds 'pong-sample` target
add_executable(process main.cpp)
target_link_libraries(process ptc)
