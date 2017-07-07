include_directories(${CMAKE_SOURCE_DIR}/../include)

link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

add_library(core STATIC hello.cpp hello.h)

add_definitions(-DEXPMODULE)