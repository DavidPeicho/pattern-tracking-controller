set(TEST_LIST ${TEST_LIST} test_surf PARENT_SCOPE)

# Adds 'pong-sample` target
add_executable(test_surf main.cpp)
target_link_libraries(test_surf ptc)
