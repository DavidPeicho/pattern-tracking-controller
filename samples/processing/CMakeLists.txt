set(SAMPLE_LIST ${SAMPLE_LIST} process PARENT_SCOPE)

# Adds 'process` target
add_executable(process main.cpp)
target_link_libraries(process ptc)
