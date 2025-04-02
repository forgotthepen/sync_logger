set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB SRC_FILES
  "src/*.cpp"
)

add_executable(playground
  ${SRC_FILES}
)

target_link_libraries(playground sync_logger)
