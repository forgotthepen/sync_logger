set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB SRC_FILES
  "src/*.cpp"
)

file(GLOB HEADER_FILES
  "include/*.hpp"
)

list(APPEND HEADER_FOLDERS
  "include/"
)

add_library(sync_logger STATIC
  ${SRC_FILES}
  ${HEADER_FILES}
)

target_include_directories(sync_logger PUBLIC
  ${HEADER_FOLDERS}
)
