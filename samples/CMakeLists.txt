add_subdirectory(processing)
add_subdirectory(pong)

add_custom_target(samples DEPENDS ${SAMPLE_LIST})
