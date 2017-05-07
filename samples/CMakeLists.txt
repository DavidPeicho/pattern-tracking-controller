add_subdirectory(game-engine)
add_subdirectory(invaders)
add_subdirectory(processing)

add_custom_target(samples DEPENDS ${SAMPLE_LIST})

# Copies assets to build dir
file(COPY ${CMAKE_SOURCE_DIR}/samples/assets/ DESTINATION ${CMAKE_BINARY_DIR}/samples/assets)
