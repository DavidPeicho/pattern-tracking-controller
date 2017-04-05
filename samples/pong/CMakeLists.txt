set(SAMPLE_LIST ${SAMPLE_LIST} pong-sample PARENT_SCOPE)

# Adds 'pong-sample` target
add_executable(pong-sample main.cpp)
target_link_libraries(pong-sample ptc)
